// Fill out your copyright notice in the Description page of Project Settings.


#include "SolarSystem.h"

// Sets default values
ASolarSystem::ASolarSystem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = root;
}

// Called when the game starts or when spawned
void ASolarSystem::BeginPlay()
{
	Super::BeginPlay();

	if (planetChunkResolution <= 1)
		planetChunkResolution = 2;

	if (waterHeight < 0)
		waterHeight = 0;

	PlacePlanets();
	BuildPlanets();
}

void ASolarSystem::PlacePlanets()
{
	FVector location = GetActorLocation();
	const FRotator rotator;
	const FActorSpawnParameters spawnParams;

	star = GetWorld()->SpawnActor<AStar>(starTemplate, location, rotator, spawnParams);
	star->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));

	for (int i = 0; i < planetCount; i++)
	{
		location += FVector(maxRadius * 200, FMath::RandRange(-maxRadius * 20, maxRadius * 20), FMath::RandRange(-maxRadius * 10, maxRadius * 10));
		APlanet* newPlanet = GetWorld()->SpawnActor<APlanet>(planetTemplate, location, rotator, spawnParams);
		planets.Add(newPlanet);
		newPlanet->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));
	}
}

void ASolarSystem::BuildPlanets()
{
	star->Init(maxRadius, starMaterial);
	celestialBodies.Add(star->body);

	for (int i=0; i<planets.Num(); i++)
	{
		float offset = i * seed;
		FVector terrainSeed = FVector(offset, offset, offset) + GetActorLocation();

		float radius = ((FMath::PerlinNoise3D(terrainSeed) + 1) * 0.5f) * maxRadius;
		int resolution = FMath::Pow(2, planetChunkResolution);

		planets[i]->Init(maxRadius, noiseSettings, waterHeight, terrainSeed, resolution, planetMaterial, waterMaterial);
		celestialBodies.Add(planets[i]->body);
	}
}

void ASolarSystem::SimulateGravity()
{
	for (auto bodyA : celestialBodies)
	{
		for (auto bodyB : celestialBodies)
		{
			if (bodyA == bodyB)
				continue;

			//FVector posA = bodyA->GetComponentLocation();
			//FVector posB = bodyB->GetComponentLocation();
			//float rSquared = FMath::Square(FVector::Distance(posA, posB));
			//
			//UE_LOG(LogTemp, Log, TEXT("R2 %f"), rSquared);
			//
			//float masses = (bodyA->mass * bodyB->mass);
			//
			//UE_LOG(LogTemp, Log, TEXT("m1m2 %f"), masses);
			//
			//float magnitude = masses / rSquared;;
			//float t = timescale * GetWorld()->DeltaTimeSeconds;
			//
			//bodyA->ApplyForce(magnitude, (posB - posA).GetSafeNormal(), t);
			//bodyB->ApplyForce(magnitude, (posA - posB).GetSafeNormal(), t);

			bodyA->ApplyForceBetween(bodyB, timescale * GetWorld()->DeltaTimeSeconds);
		}
	}
}

// Called every frame
void ASolarSystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SimulateGravity();
}

