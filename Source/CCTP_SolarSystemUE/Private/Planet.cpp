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

	//static ConstructorHelpers::FObjectFinder<UStaticMesh>SphereMeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
	//simpleMesh->SetStaticMesh(SphereMeshAsset.Object);
	//
	RootComponent = simpleMesh;

	for (int i=0; i<6; i++)
	{
		UTerrain* newTerrain = CreateDefaultSubobject<UTerrain>(TEXT("Terrain " + i));
		terrain.Add(newTerrain);

		UTerrain* newWater = CreateDefaultSubobject<UTerrain>(TEXT("Ocean " + i));
		water.Add(newWater);
	}
}

void APlanet::ManageLOD()
{
	const FVector camPos = playerCamera->GetCameraLocation();
	const FVector planetLocation = GetActorLocation();
	const float distance = FVector::Distance(camPos, planetLocation);

	FVector directionToCamera = camPos - GetActorLocation();
	directionToCamera.Normalize();

	if (distance >= 100000.f && detailLevel != 0)
	{
		currentRes = 16;
		detailLevel = 0;
	}
	else if (distance >= 50000.f && distance < 100000.f && detailLevel != 1)
	{
		currentRes = 64;
		detailLevel = 1;
	}
	else if (distance < 50000.f && distance > surfaceSettings->radius * 1.1f && detailLevel != 2)
	{
		currentRes = 128;
		detailLevel = 2;
	}

	for (int i = 0; i < terrain.Num(); i++)
	{
		terrain[i]->DetermineVisibility(planetLocation, camPos);
		terrain[i]->BuildMesh(currentRes);
	}
}

// Called when the game starts or when spawned
void APlanet::BeginPlay()
{
	Super::BeginPlay();

	//const float radius = surfaceSettings->radius;
	//simpleMesh->SetWorldScale3D(FVector(radius, radius, radius));
	//simpleMesh->SetVisibility(basicPlanet);
	SetupGravity();

	Init();
	//GenerateWater();
}

void APlanet::Init()
{
	if (UWorld* World = GetWorld())
		playerCamera = World->GetFirstPlayerController()->PlayerCameraManager;

	TArray<FVector> directions = { FVector::UpVector, FVector::DownVector,
		FVector::LeftVector, FVector::RightVector,
		FVector::ForwardVector, FVector::BackwardVector };

	for (int i=0; i<6; i++)
	{
		terrain[i]->Init(surfaceSettings, directions[i]);
		//water[i]->Init(surfaceSettings, directions[i], 16);
	}

	active = true;
}

void APlanet::GenerateWater()
{
	//for (int i=0; i<water.Num(); i++)
	//{
	//	water[i]->BuildMesh();
	//}
}

// Called every frame
void APlanet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (active)
		ManageLOD();
}

void APlanet::SetupGravity()
{
	gravityField->radius = surfaceSettings->radius * gravityOffset;// *(surfaceSettings->radius * gravityOffset);
}

