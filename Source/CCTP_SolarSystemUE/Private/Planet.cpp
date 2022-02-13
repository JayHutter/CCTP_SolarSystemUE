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
	simpleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Simple Surface"));

	RootComponent = simpleMesh;

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

	for (int i = 0; i < terrain.Num(); i++)
	{
		//terrain[i]->DetermineVisibility(planetLocation, camPos);
		//terrain[i]->BuildMesh(currentRes);
		//terrain[i]->StartBuildingMesh(currentRes);
	}
}

// Called when the game starts or when spawned
void APlanet::BeginPlay()
{
	Super::BeginPlay();

	//const float radius = surfaceSettings->radius;
	//simpleMesh->SetWorldScale3D(FVector(radius, radius, radius));
	//simpleMesh->SetVisibility(basicPlanet);

	//Init();
	//GenerateWater();
}

void APlanet::Init(float radius, TArray<FNoiseSettings> noiseSettings, int chunkResolution, UMaterialInterface* terrainMaterial, UMaterialInterface* waterMaterial)
{
	surfaceSettings->radius = radius;
	surfaceSettings->seed = GetActorLocation();
	surfaceSettings->noiseSettings = noiseSettings;
	surfaceSettings->chunkResolution = chunkResolution;

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
		UE_LOG(LogTemp, Log, TEXT("Cam: %f,%f,%f"), camLocation.X, camLocation.Y, camLocation.Z);
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

