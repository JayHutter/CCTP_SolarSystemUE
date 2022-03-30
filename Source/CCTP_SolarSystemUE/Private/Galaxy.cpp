// Fill out your copyright notice in the Description page of Project Settings.


#include "Galaxy.h"
#include "UniverseSettings.h"

// Sets default values
AGalaxy::AGalaxy()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	root = CreateDefaultSubobject<USceneComponent>("Scene Root");
	RootComponent = root;

	blackHole = CreateDefaultSubobject<UCelestialBody>(*FString("Black Hole"));
	blackHole->AttachToComponent(root, FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));
	blackHole->SetRelativeLocation(FVector::ZeroVector);

	//galaxyScale = GetActorScale().Size();

	const float blackHoleSize = 100.f * galaxyScale;
	blackHole->SetWorldScale3D(FVector(blackHoleSize, blackHoleSize, blackHoleSize));
	FMath::RandInit(galaxySeed);

	for (int i=0; i< galaxySize; i++)
	{
		UCelestialBody* newBody = CreateDefaultSubobject<UCelestialBody>(*FString("Solar System" + FString::FromInt(i)));
		bodies.Add(newBody);
	
		//float seedVal = Cast<AUniverseSettings>(GetWorldSettings())->seed * i;
		newBody->SetWorldLocation(RandomStartPosition(), false);
		newBody->AttachToComponent(root, FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));
		newBody->SetWorldScale3D(FVector(galaxyScale, galaxyScale, galaxyScale));
	}
}

// Called when the game starts or when spawned
void AGalaxy::BeginPlay()
{
	Super::BeginPlay();

	//const AUniverseSettings* universeSettings = Cast<AUniverseSettings>(GetWorldSettings());
	//const FRotator rotator;
	//const FActorSpawnParameters spawnParams;
	//
	//FMath::RandInit(galaxySeed);
	//for (int i=0; i<galaxySize; i++)
	//{
	//	AStar* newStar = GetWorld()->SpawnActor<AStar>(universeSettings->starTemplate, RandomStartPosition(), rotator, spawnParams);
	//	newStar->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));
	//	newStar->Init(universeSettings->maxRadius, universeSettings->starMaterial);
	//	bodies.Add(newStar->body);
	//}

	auto universe = Cast<AUniverseSettings>(GetWorldSettings())->universe;
	//AttachToActor(universe, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false));
	//AttachToComponent(universe->GetRootComponent(), FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));
	universe->AddGalaxy(this);

	//float scale = RootComponent->GetRelativeScale3D().Size();
	//blackHole->SetMassOverrideInKg(GetFName(), scale * scale * 1000, true);
	blackHole->SetMaterial(0, Cast<AUniverseSettings>(GetWorldSettings())->starMaterial);
	for (auto body : bodies)
	{
		body->Init(0.25f, 1);
		body->SetMaterial(0, Cast<AUniverseSettings>(GetWorldSettings())->starMaterial);
	}

	//SetInitialVelocities();
	SetSystemScale();
	SetInitialVelocitiesSimple();
}

FVector AGalaxy::RandomStartPosition()
{
	float r = galaxyRadius * galaxyScale * FMath::Sqrt(FMath::RandRange(0.f, 1.f));
	float theta = FMath::RandRange(0.f, 1.f) * 2.f * PI;

	r += 1000 * galaxyScale;

	float x = RootComponent->GetComponentLocation().X + r * FMath::Cos(theta);
	float y = RootComponent->GetComponentLocation().Y + r * FMath::Sin(theta);
	float z = RootComponent->GetComponentLocation().Z + r * FMath::Sin(theta);

	return FVector(x, y, z);
}

FVector AGalaxy::RandomStartPosition(int i)
{
	FMath::RandInit(galaxySeed + i);
	float r = galaxyRadius * FMath::Sqrt(FMath::RandRange(0.f, 1.f));
	float theta = FMath::RandRange(0.f, 1.f) * 2.f * PI;

	r += minDistance;

	float x = RootComponent->GetComponentLocation().X + r * FMath::Cos(theta);
	float y = RootComponent->GetComponentLocation().Y + r * FMath::Sin(theta);
	float z = RootComponent->GetComponentLocation().Z + r * FMath::Sin(theta);

	return FVector(x, y, z);
}

FVector AGalaxy::RandomStartPosition(FVector seed)
{
	UE_LOG(LogTemp, Log, TEXT("Seed %f, %f, %f"), seed.X, seed.Y, seed.Z);

	const float val = (FMath::PerlinNoise3D(seed) + 1) * .5f;
	const float r = galaxyRadius * FMath::Sqrt(val);
	const float theta = val * 2.f * PI;

	float x = RootComponent->GetComponentLocation().X + r * FMath::Cos(theta);
	float y = RootComponent->GetComponentLocation().Y + r * FMath::Sin(theta);
	float z = RootComponent->GetComponentLocation().Z + r * FMath::Sin(theta);

	UE_LOG(LogTemp, Log, TEXT("Pos %f, %f, %f"), x, y, z);

	return FVector(x, y, z);
}

// Called every frame
void AGalaxy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//SimulateGravity();
	SimulateGravitySimple();
	UpdateSolarSystemLocation();
}

ASolarSystem* AGalaxy::LoadSolarSystem(int index)
{
	if (index > bodies.Num())
		return loadedSolarSystem;

	if (loadedSolarSystem)
		UnloadSolarSystem();

	FActorSpawnParameters spawnParams;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	const FRotator rotator;
	const AUniverseSettings* universeSettings = Cast<AUniverseSettings>(GetWorldSettings());

	UCelestialBody* body = bodies[index];

	ASolarSystem* newSystem = GetWorld()->SpawnActor<ASolarSystem>(universeSettings->solarSystemTemplate, body->GetComponentLocation(), rotator, spawnParams);
	//newSystem->AttachToComponent(body, FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));
	auto loc = body->GetComponentLocation();
	UE_LOG(LogTemp, Log, TEXT("Body %f, %f, %f"), loc.X, loc.Y, loc.Z);
	newSystem->GenerateSolarSystem(galaxySeed + index);

	loadedSolarSystem = newSystem;
	loadedSystemBody = body;
	//loadedSystemId = index;

	//body->SetVisibility(false);

	return loadedSolarSystem;
}

void AGalaxy::UnloadSolarSystem()
{
	//UCelestialBody* body = bodies[loadedSystemId];
	loadedSystemBody->SetVisibility(true);
	loadedSystemBody = nullptr;
	//loadedSystemId = -1;

	loadedSolarSystem->DestroySystem();
	loadedSolarSystem = nullptr;
}

//Updates the solar system position to match the simulation location
void AGalaxy::UpdateSolarSystemLocation()
{
	if (!loadedSystemBody || !loadedSolarSystem)
		return;

	loadedSolarSystem->SetActorLocation(loadedSystemBody->GetComponentLocation());
}

/* Teleports the entire galaxy to a new location
 * Each component must be manually updated due to physics enabled
 */
void AGalaxy::MoveGalaxy(FVector location)
{
	const FVector galaxyLoc = GetActorLocation();

	FVector bHoleRelLoc = blackHole->GetComponentLocation() - galaxyLoc;
	blackHole->SetWorldLocation(location + bHoleRelLoc);

	for (auto body : bodies)
	{
		FVector relLoc = body->GetComponentLocation() - galaxyLoc;
		body->SetWorldLocation(location + relLoc);
	}
	
	SetActorLocation(location);
}

int AGalaxy::GetTotalSolarSystems()
{
	return bodies.Num();
}

void AGalaxy::SetSystemScale()
{
	blackHole->scale = galaxyScale;
	blackHole->SetMassOverrideInKg(GetFName(), 1000000000, true);
	for (auto body : bodies)
	{
		body->SetMassOverrideInKg(GetFName(), 100, true);
		body->scale = galaxyScale;
	}
}

//Only simulate the solar systems against the black hole to save on computation
void AGalaxy::SimulateGravitySimple()
{
	for (int i = 0; i < galaxySize; i++)
	{
		bodies[i]->ApplyForceBetween(blackHole);
		//systems[i]->solarBody->ApplyForceBetween(blackHole);
	}
}

//Only determines the cetripetal force against the black hole
void AGalaxy::SetInitialVelocitiesSimple()
{
	for (int i=0; i< galaxySize; i++)
	{
		bodies[i]->SetInitialVelocity(blackHole);
		//systems[i]->solarBody->SetInitialVelocity(blackHole);
	}
}

