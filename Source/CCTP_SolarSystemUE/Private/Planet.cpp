// Fill out your copyright notice in the Description page of Project Settings.


#include "Planet.h"

#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "Kismet\KismetSystemLibrary.h"

// Sets default values
APlanet::APlanet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	surfaceSettings = CreateDefaultSubobject<USurfaceSettings>(TEXT("Surface Settings"));
	gravityField = CreateDefaultSubobject<UGravitationalField>(TEXT("Gravitational Field"));
	body = CreateDefaultSubobject<UCelestialBody>(TEXT("Body"));

	RootComponent = body;

	for (int i=0; i<6; i++)
	{
		UTerrain* newTerrain = CreateDefaultSubobject<UTerrain>(*FString("Terrain" + FString::FromInt(i)));
		terrain.Add(newTerrain);
	}
}

void APlanet::ManageLOD()
{
	const FVector camPos = playerCamera->GetCameraLocation();
	const FVector planetLocation = GetActorLocation();
}

// Called when the game starts or when spawned
void APlanet::BeginPlay()
{
	Super::BeginPlay();
}

void APlanet::Init(float radius, TArray<FNoiseSettings> noiseSettings, float waterHeight, FVector generationSeed, int chunkResolution, UMaterialInterface* terrainMaterial, UMaterialInterface* waterMaterial)
{
	surfaceSettings->radius = radius;
	surfaceSettings->seed = generationSeed;
	surfaceSettings->noiseSettings = noiseSettings;
	surfaceSettings->chunkResolution = chunkResolution;
	surfaceSettings->waterHeight = waterHeight;

	if (UWorld* World = GetWorld())
		playerCamera = World->GetFirstPlayerController()->PlayerCameraManager;

	TArray<FVector> directions = { FVector::UpVector, FVector::DownVector,
		FVector::LeftVector, FVector::RightVector,
		FVector::ForwardVector, FVector::BackwardVector };

	for (int i=0; i<6; i++)
	{
		terrain[i]->Init(surfaceSettings, directions[i], terrainMaterial, waterMaterial);
	}
	SetupGravity();
	active = true;

	body->mass = radius * radius * radius;
}

// Called every frame
void APlanet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//if (active)
	//	ManageLOD();
	if (active && timer >= 1.f)
	{
		FVector camLocation = playerCamera->GetCameraLocation();
		GenerateTerrain();
		timer = 0;
	}
	timer += GetWorld()->GetDeltaSeconds();
}

void APlanet::SetupGravity()
{
	gravityField->radius = surfaceSettings->radius * gravityOffset;// *(surfaceSettings->radius * gravityOffset);
}

void APlanet::GenerateTerrain()
{
	for (auto &t : terrain)
	{
		t->ConstructQuadTree();
	}
}