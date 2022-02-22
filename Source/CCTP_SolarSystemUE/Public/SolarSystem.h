// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Planet.h"
#include "Star.h"
#include "SurfaceSettings.h"
#include "SolarSystem.generated.h"

UCLASS()
class CCTP_SOLARSYSTEMUE_API ASolarSystem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASolarSystem();

	UPROPERTY(EditAnywhere)
	TSubclassOf<APlanet>  planetTemplate;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AStar> starTemplate;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ASatellite> satelliteTemplate;

	UPROPERTY(VisibleAnywhere)
	AStar* star;

	UPROPERTY(VisibleAnywhere)
	TArray<APlanet*> planets;

	UPROPERTY(EditAnywhere)
	int planetCount = 1;
	UPROPERTY(EditAnywhere)
	float miniumRadius = 10000.f;
	UPROPERTY(EditAnywhere)
	float maxRadius = 20000.f;

	UPROPERTY(EditAnywhere)
	int planetChunkResolution = 8;

	UPROPERTY(EditAnywhere)
	float seed = 1000.f;

	UPROPERTY(EditAnywhere)
	float waterHeight=0.5f;

	UPROPERTY(EditAnywhere)
	TArray<FNoiseSettings> noiseSettings;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* planetMaterial;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* waterMaterial;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* starMaterial;

	UPROPERTY(EditAnywhere)
	float massScale = 10000;

	UPROPERTY(EditAnywhere)
	float gravitationalConstant = 10000;

	UPROPERTY(EditAnywhere)
	float timescale = 1.f;

	TArray<UCelestialBody*> celestialBodies;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void PlacePlanets();
	void BuildPlanets();
	void SimulateGravity();
	void SetInitialVelocities();

	USceneComponent* root;
	bool setup = false;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
