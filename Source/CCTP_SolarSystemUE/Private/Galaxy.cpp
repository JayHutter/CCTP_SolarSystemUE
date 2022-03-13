// Fill out your copyright notice in the Description page of Project Settings.


#include "Galaxy.h"
#include "UniverseSettings.h"

// Sets default values
AGalaxy::AGalaxy()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	UCelestialBody* blackHole = CreateDefaultSubobject<UCelestialBody>(*FString("Black Hole"));
	blackHole->SetWorldScale3D(FVector(100, 100, 100));
	RootComponent = blackHole;
	bodies.Add(blackHole);

	for (int i=0; i< galaxySize; i++)
	{
		UCelestialBody* newBody = CreateDefaultSubobject<UCelestialBody>(*FString("Solar System" + FString::FromInt(i)));
		bodies.Add(newBody);

		newBody->SetWorldLocation(RandomStartPosition(), false);
	}
}

// Called when the game starts or when spawned
void AGalaxy::BeginPlay()
{
	Super::BeginPlay();

	for (auto body : bodies)
	{
		body->SetMaterial(0, Cast<AUniverseSettings>(GetWorldSettings())->starMaterial);
	}

	//SetInitialVelocities();
	SetInitialVelocitiesSimple();
}

FVector AGalaxy::RandomStartPosition()
{
	float r = galaxyRadius * FMath::Sqrt(FMath::RandRange(0.f, 1.f));
	float theta = FMath::RandRange(0.f, 1.f) * 2.f * PI;

	float x = RootComponent->GetComponentLocation().X + r * FMath::Cos(theta);
	float y = RootComponent->GetComponentLocation().Y + r * FMath::Sin(theta);
	float z = RootComponent->GetComponentLocation().Z + r * FMath::Sin(theta);

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
	auto bodyA = bodies[bodyAId];
	auto bodyB = bodies[bodyBId];

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

//Only simulate the solar systems against the black hole to save on computation
void AGalaxy::SimulateGravitySimple()
{
	for (int i = 1; i <= galaxySize; i++)
	{
		bodies[i]->ApplyForceBetween(bodies[0]);
	}
}

//Only determines the cetripetal force against the black hole
void AGalaxy::SetInitialVelocitiesSimple()
{
	for (int i=1; i<= galaxySize; i++)
	{
		bodies[i]->SetInitialVelocity(bodies[0]);
	}
}

