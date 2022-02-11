// Fill out your copyright notice in the Description page of Project Settings.


#include "SolarSystem.h"

// Sets default values
ASolarSystem::ASolarSystem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


}

// Called when the game starts or when spawned
void ASolarSystem::BeginPlay()
{
	Super::BeginPlay();

	PlacePlanets();
	BuildPlanets();
}

void ASolarSystem::PlacePlanets()
{
	FVector location = GetActorLocation();
	for (int i = 0; i < planetCount; i++)
	{
		FActorSpawnParameters spawnParams;
		FRotator rotator;

		APlanet* newPlanet = GetWorld()->SpawnActor<APlanet>(planetTemplate, location, rotator, spawnParams);
		planets.Add(newPlanet);

		location += FVector(maxRadius * 10, FMath::RandRange(-maxRadius*20, maxRadius*20), FMath::RandRange(-maxRadius*10, maxRadius*10));
	}
}

void ASolarSystem::BuildPlanets()
{
	for (int i=0; i<planets.Num(); i++)
	{
		planets[i]->Init(miniumRadius, noiseSettings);
	}
}

// Called every frame
void ASolarSystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

