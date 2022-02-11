// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Planet.h"
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

	UPROPERTY(VisibleAnywhere)
	TArray<APlanet*> planets;

	UPROPERTY(EditAnywhere)
	int planetCount = 1;
	UPROPERTY(EditAnywhere)
	float miniumRadius = 10000.f;
	UPROPERTY(EditAnywhere)
	float maxRadius = 20000.f;

	UPROPERTY(EditAnywhere)
	TArray<FNoiseSettings> noiseSettings;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void PlacePlanets();
	void BuildPlanets();
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
