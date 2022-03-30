// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Galaxy.h"
#include "GameFramework/Actor.h"
#include "Universe.generated.h"

UCLASS()
class CCTP_SOLARSYSTEMUE_API AUniverse : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AUniverse();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* sceneComponent;

	TArray<AGalaxy*> galaxies;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void UpdatePostion(FVector playerPosition);
	void SetCentrePoint(FVector centrePoint);
	void SetUniversePosition(FVector newLocation);

	void AddGalaxy(AGalaxy* galaxy);
	void MoveAllGalaxies(FVector newUniLoc);
};
