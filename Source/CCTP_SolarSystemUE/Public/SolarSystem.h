// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Planet.h"
#include "Star.h"
#include "SurfaceSettings.h"
#include "SolarSystem.generated.h"

USTRUCT()
struct FOldSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<FNoiseSettings> noiseSettings;
};

UCLASS()
class CCTP_SOLARSYSTEMUE_API ASolarSystem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASolarSystem();
	UPROPERTY(VisibleAnywhere)
	AStar* star;
	
	UPROPERTY(VisibleAnywhere)
	TArray<APlanet*> planets;
	
	UPROPERTY(VisibleAnywhere)
	TArray<APlanet*> satellites;
	//
	UPROPERTY(EditAnywhere)
	int planetCount = 1;

	UPROPERTY(EditAnywhere)
	int solarSystemSeed;

	TArray<UCelestialBody*> celestialBodies;
	UPROPERTY(VisibleAnywhere)
	USceneComponent* planetSpawnAnchor;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* planetSpawner;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void PlacePlanets();
	void SimulateGravity();
	void SetInitialVelocities();

	USceneComponent* root;
	bool setup = false;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UCelestialBody* solarBody;
};
