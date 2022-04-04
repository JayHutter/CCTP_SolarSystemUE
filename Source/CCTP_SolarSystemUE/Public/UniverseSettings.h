// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/WorldSettings.h"
#include "SurfaceSettings.h"
#include "Planet.h"
#include "SolarSystem.h"
#include "Star.h"
#include "Universe.h"
#include "UniverseSettings.generated.h"

/**
 * 
 */
UCLASS()
class CCTP_SOLARSYSTEMUE_API AUniverseSettings : public AWorldSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	float miniumRadius = 10000000.f;

	UPROPERTY(EditAnywhere)
	float maxRadius = 100000000.f;

	UPROPERTY(EditAnywhere)
	float planetDistance = 500.f;

	UPROPERTY(EditAnywhere)
	int planetChunkResolution = 4;

	UPROPERTY(EditAnywhere)
	float seed = 123;

	UPROPERTY(EditAnywhere)
	int minPlanetCount = 3;

	UPROPERTY(EditAnywhere)
	int maxPlanetCount = 10;

	UPROPERTY(EditAnywhere)
	TArray<FPlanetSettings> planetSettings;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* planetMaterial;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* waterMaterial;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* starMaterial;

	UPROPERTY(EditAnywhere)
	float massScale = 1;

	UPROPERTY(EditAnywhere)
	float gravitationalConstant = 10;

	UPROPERTY(EditAnywhere)
	TSubclassOf<APlanet>  planetTemplate;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AStar> starTemplate;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ASolarSystem> solarSystemTemplate;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UObject> satelliteTemplate;

	AUniverse* universe;

	static FVector GetRandomLocationInCircle(float radius, float minimumDistance = 0.f);
	static FVector GetRandomLocationOnCircle(float radius);

	static FVector GetRandomLocationOnSphere(float radius);
};
