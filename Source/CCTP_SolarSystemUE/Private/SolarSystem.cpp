// Fill out your copyright notice in the Description page of Project Settings.


#include "SolarSystem.h"

#include "Kismet/GameplayStatics.h"
#include "UniverseSettings.h"

// Sets default values
ASolarSystem::ASolarSystem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	solarBody = CreateDefaultSubobject<UCelestialBody>(TEXT("Solar System BodY"));
	solarBody->SetVisibility(false);
	RootComponent = root;
	planetSpawnAnchor = CreateDefaultSubobject<USceneComponent>(TEXT("Planet Spawn Root"));
	planetSpawner = CreateDefaultSubobject<USceneComponent>(TEXT("Planet Spawn Point"));
	planetSpawner->AttachToComponent(planetSpawnAnchor, FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));
}

void ASolarSystem::GenerateSolarSystem(int seed)
{
	UE_LOG(LogTemp, Log, TEXT("Generating Solar Systems"));

	if (seed >= 0)
		solarSystemSeed = seed;

	PlacePlanets();
	SetInitialVelocities();
}

// Called when the game starts or when spawned
void ASolarSystem::BeginPlay()
{
	Super::BeginPlay();
	//if (generateAtStart)
	//	GenerateSolarSystem();
}

void ASolarSystem::PlacePlanets()
{
	const AUniverseSettings* universeSettings = Cast<AUniverseSettings>(GetWorldSettings());

	FMath::RandInit(solarSystemSeed);
	planetCount = FMath::RandRange(universeSettings->minPlanetCount, universeSettings->maxPlanetCount);

	FVector location = GetActorLocation();
	const FRotator rotator;
	const FActorSpawnParameters spawnParams;

	star = GetWorld()->SpawnActor<AStar>(universeSettings->starTemplate, location, rotator, spawnParams);
	star->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));
	star->SetActorLocation(location);

	star->Init(universeSettings->maxRadius, universeSettings->starMaterial);
	celestialBodies.Add(star->body);

	for (int i = 0; i < planetCount; i++)
	{
		float universeSeed = universeSettings->seed;
		float offset = i * universeSeed;
		//FVector planetSeed =  solarSystemSeed + FVector(offset, offset, offset);

		planetSpawner->SetWorldLocation(FVector(universeSettings->maxRadius * universeSettings->planetDistance, 0, 0) +
			planetSpawner->GetComponentLocation());

		//float rotation = FMath::PerlinNoise3D(planetSeed / (universeSeed *3.f)) * 360.f;

		float planetSeed = solarSystemSeed + offset;
		FMath::RandInit(planetSeed);
		float rotation = FMath::RandRange(0.f, 360.f);

		//AddActorLocalRotation(FRotator(0, rotation, 0));
		planetSpawnAnchor->AddLocalRotation(FRotator(0, rotation, 0));

		APlanet* newPlanet = GetWorld()->SpawnActor<APlanet>(universeSettings->planetTemplate, planetSpawner->GetComponentLocation(), rotator, spawnParams);
		planets.Add(newPlanet);
		newPlanet->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));

		//Needs to be pseudo-random rather than patterned like this
		int noiseId = i % universeSettings->planetSettings.Num();
		FPlanetSettings newPlanetSettings = universeSettings->planetSettings[noiseId];

		//newPlanetSettings.radius = ((FMath::PerlinNoise3D(planetSeed / (universeSeed * 3.f)) + 1) * 0.5f) * universeSettings->maxRadius;
		newPlanetSettings.radius = FMath::RandRange(universeSettings->miniumRadius, universeSettings->maxRadius);
		newPlanetSettings.chunkResolution = FMath::Pow(2, universeSettings->planetChunkResolution);
		newPlanetSettings.seed = FVector(planetSeed, planetSeed, planetSeed);
		newPlanetSettings.noiseScale = newPlanetSettings.radius / universeSettings->miniumRadius;

		newPlanet->Init(newPlanetSettings, newPlanetSettings.surfaceMaterial, universeSettings->waterMaterial);
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
		if (bodyA == celestialBodies[0])
			continue;

		for (auto bodyB : celestialBodies)
		{

			if (bodyA == bodyB)
				continue;

			bodyA->ApplyForceBetween(bodyB);
		}
	}
}

void ASolarSystem::SetInitialVelocities()
{
	for (auto bodyA : celestialBodies)
	{
		if (bodyA == celestialBodies[0])
			continue;

		for (auto bodyB : celestialBodies)
		{
			if (bodyA == bodyB)
				continue;
	
			bodyA->SetInitialVelocity(bodyB);
		}
	}
}

// Called every frame
void ASolarSystem::Tick(float DeltaTime)	
{
	Super::Tick(DeltaTime);
	SimulateGravity();
}

