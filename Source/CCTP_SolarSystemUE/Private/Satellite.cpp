// Fill out your copyright notice in the Description page of Project Settings.


#include "Satellite.h"

#include "UniverseSettings.h"

// Sets default values
ASatellite::ASatellite()
{
	body = CreateDefaultSubobject<UCelestialBody>(TEXT("Body"));
	//body->SetWorldScale3D(FVector::OneVector * 100000.f);
	
	RootComponent = body;

	for (int i = 0; i < 6; i++)
	{
		UTerrain* newTerrain = CreateDefaultSubobject<UTerrain>(*FString("Terrain" + FString::FromInt(i)));
		terrain.Add(newTerrain);
	}

	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ASatellite::BeginPlay()
{
	Super::BeginPlay();
	body->Init(10000000000000.f, 1);
	body->SetMassOverrideInKg(GetFName(), 10000000.f, true);

	TArray<FVector> directions = { FVector::UpVector, FVector::DownVector,
		FVector::LeftVector, FVector::RightVector,
		FVector::ForwardVector, FVector::BackwardVector };

	FPlanetSettings settings = Cast<AUniverseSettings>(GetWorldSettings())->satelliteSettings;

	for (int i = 0; i < 6; i++)
	{
		terrain[i]->Init(&settings, RootComponent, directions[i], settings.surfaceMaterial);
	}
	GenerateTerrain();
}

// Called every frame
void ASatellite::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (timer >= 1.f)
	{
		GenerateTerrain();
		timer = 0;
	}

	timer += GetWorld()->GetDeltaSeconds();
}

//Move the satellite by the same amount as the planet has moved since last update
void ASatellite::MoveWithParent(FVector currentLocation)
{
	FVector movement = currentLocation - prevLocation;
	AddActorWorldOffset(movement);

	prevLocation = currentLocation;
}

void ASatellite::GenerateTerrain()
{
	for (auto& t : terrain)
	{
		t->ConstructQuadTree();
	}
}

