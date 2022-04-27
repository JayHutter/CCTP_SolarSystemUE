// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CelestialBody.h"
#include "Terrain.h"
#include "GameFramework/Actor.h"
#include "Satellite.generated.h"

UCLASS()
class CCTP_SOLARSYSTEMUE_API ASatellite : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASatellite();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void MoveWithParent(FVector currentLocation);

	UPROPERTY(EditAnywhere)
	UCelestialBody* body;
	FVector prevLocation;

	UPROPERTY(VisibleAnywhere, Instanced)
	TArray<UTerrain*> terrain;

	void GenerateTerrain();
	float timer = 0;
};
