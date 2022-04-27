// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CelestialBody.h"
//#include "PlayerCharacter.h"
#include "SolarSystem.h"
#include "Galaxy.generated.h"

UCLASS()
class CCTP_SOLARSYSTEMUE_API AGalaxy : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGalaxy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	FVector RandomStartPosition();

	void SetSystemScale();
	void SimulateGravitySimple();
	void SetInitialVelocitiesSimple();

	UPROPERTY(VisibleAnywhere)
	int bodyAId = 0;
	UPROPERTY(VisibleAnywhere)
	int bodyBId = 0;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	ASolarSystem* LoadSolarSystem(int index);

	UFUNCTION(BlueprintCallable)
	void UnloadSolarSystem();
	void UpdateSolarSystemLocation();
	void MoveGalaxy(FVector location);

	void PauseSimulation();
	void ResumeSimulation();

	//UFUNCTION(BlueprintCallable)
	//void AddPlayer(APlayerCharacter* playerPtr);

	UFUNCTION(BlueprintCallable)
	int GetTotalSolarSystems();

	UPROPERTY(EditAnywhere)
	int galaxySize = 3000;

	UPROPERTY(EditAnywhere)
	float galaxyRadius = 100000.f;

	UPROPERTY(EditAnywhere)
	float minDistance = 10000.f;

	float galaxyScale = 500000000.f;
	float starScale = 100000000.f;

	UPROPERTY(EditAnywhere)
	int galaxySeed;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* root;

	UPROPERTY(VisibleAnywhere)
	UCelestialBody* blackHole;
	TArray<UCelestialBody*> bodies;
	TArray<AStar*> stars;
	//APlayerCharacter* player;

	//int loadedSystemId = -1;

	UPROPERTY(VisibleAnywhere)
	ASolarSystem* loadedSolarSystem = nullptr;
	UCelestialBody* loadedSystemBody = nullptr;

	UPROPERTY(EditAnywhere)
	bool pauseOnSystemLoad = false;

	UPROPERTY(VisibleAnywhere);
	bool simulating = true;
};