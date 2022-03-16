// Fill out your copyright notice in the Description page of Project Settings.


#include "Galaxy.h"
#include "UniverseSettings.h"

// Sets default values
AGalaxy::AGalaxy()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	blackHole = CreateDefaultSubobject<UCelestialBody>(*FString("Black Hole"));
	
	RootComponent = CreateDefaultSubobject<USceneComponent>("Scene Root");

	blackHole->SetRelativeLocation(FVector::ZeroVector);
	blackHole->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));
	blackHole->SetWorldScale3D(FVector(100, 100, 100));
	FMath::RandInit(galaxySeed);
	for (int i=0; i< galaxySize; i++)
	{
		UCelestialBody* newBody = CreateDefaultSubobject<UCelestialBody>(*FString("Solar System" + FString::FromInt(i)));
		bodies.Add(newBody);
	
		//float seedVal = Cast<AUniverseSettings>(GetWorldSettings())->seed * i;
		newBody->SetWorldLocation(RandomStartPosition(), false);
		newBody->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));
	}
}

// Called when the game starts or when spawned
void AGalaxy::BeginPlay()
{
	Super::BeginPlay();
	//float scale = RootComponent->GetRelativeScale3D().Size();
	//blackHole->SetMassOverrideInKg(GetFName(), scale * scale * 1000, true);
	blackHole->SetMaterial(0, Cast<AUniverseSettings>(GetWorldSettings())->starMaterial);
	for (auto body : bodies)
	{
		body->SetMaterial(0, Cast<AUniverseSettings>(GetWorldSettings())->starMaterial);
	}

	//SetInitialVelocities();
	SetSystemScale();
	SetInitialVelocitiesSimple();
}

FVector AGalaxy::RandomStartPosition()
{
	float r = galaxyRadius * FMath::Sqrt(FMath::RandRange(0.f, 1.f));
	float theta = FMath::RandRange(0.f, 1.f) * 2.f * PI;

	r += 1000;

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

	r += 1000;

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
}

void AGalaxy::SimulateGravity()
{
	//auto bodyA = bodies[bodyAId];
	//auto bodyB = bodies[bodyBId];

	auto bodyA = systems[bodyAId]->solarBody;
	auto bodyB = systems[bodyBId]->solarBody;

	if (bodyA != bodyB)
		bodyA->ApplyForceBetween(bodyB);

	bodyBId++;
	if (bodyBId >= bodies.Num())
	{
		bodyAId++;
		bodyAId %= bodies.Num();
		bodyBId = 0;
	}
}

void AGalaxy::SetInitialVelocities()
{
	for (auto bodyA : bodies)
	{
		for (auto bodyB : bodies)
		{
			if (bodyA == bodyB)
				continue;

			bodyA->SetInitialVelocity(bodyB);
		}
	}
}

void AGalaxy::SetSystemScale()
{
	float scale = GetActorScale().Size();
	blackHole->scale = scale;
	blackHole->SetMassOverrideInKg(GetFName(), 1000000000, true);
	for (auto body : bodies)
	{
		body->SetMassOverrideInKg(GetFName(), 100, true);
		body->scale = scale;
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

