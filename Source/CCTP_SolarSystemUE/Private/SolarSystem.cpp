// Fill out your copyright notice in the Description page of Project Settings.


#include "SolarSystem.h"

#include "Kismet/GameplayStatics.h"
#include "UniverseSettings.h"

// Sets default values
ASolarSystem::ASolarSystem()
{
	root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	solarBody = CreateDefaultSubobject<UCelestialBody>(TEXT("Solar System BodY"));
	solarBody->SetVisibility(false);
	RootComponent = root;
	planetSpawnAnchor = CreateDefaultSubobject<USceneComponent>(TEXT("Planet Spawn Root"));
	//planetSpawnAnchor->AttachToComponent(planetSpawnAnchor, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true));
	planetSpawnAnchor->SetupAttachment(root);
	planetSpawner = CreateDefaultSubobject<USceneComponent>(TEXT("Planet Spawn Point"));
	//planetSpawner->AttachToComponent(planetSpawnAnchor, FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));
	planetSpawner->SetupAttachment(planetSpawnAnchor);

	PrimaryActorTick.bCanEverTick = true;
}

void ASolarSystem::GenerateSolarSystem(int seed)
{
	if (seed >= 0)
		solarSystemSeed = seed;

	PlacePlanets();
	SetInitialVelocities();
}

// Called when the game starts or when spawned
void ASolarSystem::BeginPlay()
{
	Super::BeginPlay();
	if (generateAtStart)
		GenerateSolarSystem();
}

void ASolarSystem::PlacePlanets()
{
	auto loc = GetActorLocation();
	UE_LOG(LogTemp, Log, TEXT("Loc %f, %f, %f"), loc.X, loc.Y, loc.Z);

	const AUniverseSettings* universeSettings = Cast<AUniverseSettings>(GetWorldSettings());

	FMath::RandInit(solarSystemSeed);
	planetCount = FMath::RandRange(universeSettings->minPlanetCount, universeSettings->maxPlanetCount);

	FVector location = GetActorLocation();
	const FRotator rotator;
	const FActorSpawnParameters spawnParams;

	star = GetWorld()->SpawnActor<AStar>(universeSettings->starTemplate, location, rotator, spawnParams);
	star->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));
	star->SetActorLocation(location);
	planetSpawnAnchor->SetWorldLocation(location);
	planetSpawner->SetWorldLocation(location);

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

		int noiseId = i % universeSettings->planetSettings.Num();
		FPlanetSettings newPlanetSettings = universeSettings->planetSettings[noiseId];

		//newPlanetSettings.radius = ((FMath::PerlinNoise3D(planetSeed / (universeSeed * 3.f)) + 1) * 0.5f) * universeSettings->maxRadius;
		newPlanetSettings.radius = FMath::RandRange(universeSettings->miniumRadius, universeSettings->maxRadius);
		newPlanetSettings.chunkResolution = FMath::Pow(2, universeSettings->planetChunkResolution);
		newPlanetSettings.seed = FVector(planetSeed, planetSeed, planetSeed);
		newPlanetSettings.noiseScale = newPlanetSettings.radius / universeSettings->miniumRadius;
		newPlanetSettings.surfaceMaterial = universeSettings->planetMaterial;
		const int gradId = FMath::RandRange(0, universeSettings->terrainGradients.Num()-1);
		newPlanetSettings.terrainGradient = universeSettings->terrainGradients[gradId];

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

FVector ASolarSystem::GetTeleportPoint()
{
	return planets[planets.Num()-1]->GetPointOnSurface();
}

void ASolarSystem::TeleportPlayerTo()
{
	auto universe = Cast<AUniverseSettings>(GetWorldSettings())->universe;

	//universe->SetActorLocation(-GetActorLocation());
	universe->SetUniversePosition(-GetActorLocation());
}

//Destroys all the objects in the system before destroying the system itself
void ASolarSystem::DestroySystem()
{
	star->Destroy();

	for (auto planet : planets)
	{
		planet->DestroyPlanet();
	}

	Destroy();
}

