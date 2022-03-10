// Fill out your copyright notice in the Description page of Project Settings.


#include "SolarSystem.h"

#include "Kismet/GameplayStatics.h"

// Sets default values
ASolarSystem::ASolarSystem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = root;
	planetSpawnAnchor = CreateDefaultSubobject<USceneComponent>(TEXT("Planet Spawn Root"));
	planetSpawner = CreateDefaultSubobject<USceneComponent>(TEXT("Planet Spawn Point"));
	planetSpawner->AttachToComponent(planetSpawnAnchor, FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));
}

// Called when the game starts or when spawned
void ASolarSystem::BeginPlay()
{
	Super::BeginPlay();

	if (planetChunkResolution <= 1)
		planetChunkResolution = 2;

	PlacePlanets();
	SetInitialVelocities();
	if (planetDistance < 10)
		planetDistance = 10;
}

void ASolarSystem::PlacePlanets()
{
	FVector location = GetActorLocation();
	const FRotator rotator;
	const FActorSpawnParameters spawnParams;

	star = GetWorld()->SpawnActor<AStar>(starTemplate, location, rotator, spawnParams);
	star->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));

	star->Init(maxRadius, starMaterial);
	celestialBodies.Add(star->body);

	for (int i = 0; i < planetCount; i++)
	{
		float offset = i * seed;
		FVector planetSeed =  GetActorLocation() + FVector(offset, offset, offset);

		planetSpawner->SetWorldLocation(FVector(maxRadius * planetDistance, 0, 0) +
			planetSpawner->GetComponentLocation());

		float rotation = FMath::PerlinNoise3D(planetSeed / (seed*3.f)) * 360.f;

		//AddActorLocalRotation(FRotator(0, rotation, 0));
		planetSpawnAnchor->AddLocalRotation(FRotator(0, rotation, 0));

		APlanet* newPlanet = GetWorld()->SpawnActor<APlanet>(planetTemplate, planetSpawner->GetComponentLocation(), rotator, spawnParams);
		planets.Add(newPlanet);
		newPlanet->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));

		//Needs to be pseudo-random rather than patterned like this
		int noiseId = i % planetSettings.Num();

		planetSettings[noiseId].radius = ((FMath::PerlinNoise3D(planetSeed / (seed * 3.f)) + 1) * 0.5f) * maxRadius;
		planetSettings[noiseId].chunkResolution = FMath::Pow(2, planetChunkResolution);
		planetSettings[noiseId].seed = planetSeed;

		newPlanet->Init(planetSettings[noiseId], planetMaterial, waterMaterial);
		celestialBodies.Add(newPlanet->body);

		//Satellites currently fall off - need to look into
		//location += FVector(maxRadius*2.f, 0, 0);
		//APlanet* testSat = GetWorld()->SpawnActor<APlanet>(planetTemplate, location, rotator, spawnParams);
		//satellites.Add(testSat);
		//testSat->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));
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

			bodyA->ApplyForceBetween(bodyB, gravitationalConstant, massScale);
		}
	}
}

void ASolarSystem::SetInitialVelocities()
{
	for (auto bodyA : celestialBodies)
	{
		for (auto bodyB : celestialBodies)
		{
			if (bodyA == bodyB)
				continue;
	
			bodyA->SetInitialVelocity(bodyB, gravitationalConstant, massScale);
		}
	}
}

// Called every frame
void ASolarSystem::Tick(float DeltaTime)	
{
	Super::Tick(DeltaTime);
	SimulateGravity();
}

