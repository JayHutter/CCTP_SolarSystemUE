// Fill out your copyright notice in the Description page of Project Settings.


#include "Planet.h"

#include "UniverseSettings.h"
#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Kismet\KismetSystemLibrary.h"

// Sets default values
APlanet::APlanet()
{
	//gravityField = CreateDefaultSubobject<UGravitationalField>(TEXT("Gravitational Field"));
	body = CreateDefaultSubobject<UCelestialBody>(TEXT("Body"));

	RootComponent = body;

	satelliteRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Satellite Follow Point"));

	for (int i=0; i<6; i++)
	{
		UTerrain* newTerrain = CreateDefaultSubobject<UTerrain>(*FString("Terrain" + FString::FromInt(i)));
		terrain.Add(newTerrain);
	}

	PrimaryActorTick.bCanEverTick = true;
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

FVector APlanet::GetPointOnSurface()
{
	//FVector location = RootComponent->GetRelativeLocation();
	FVector location = GetActorLocation();
	location += GetActorUpVector() * planetSettings.radius * 1.2f;
	return location;
}

void APlanet::Init(FPlanetSettings settings, UMaterialInterface* terrainMaterial, UMaterialInterface* waterMaterial)
{
	planetSettings = settings;

	if (UWorld* World = GetWorld())
		playerCamera = World->GetFirstPlayerController()->PlayerCameraManager;

	TArray<FVector> directions = { FVector::UpVector, FVector::DownVector,
		FVector::LeftVector, FVector::RightVector,
		FVector::ForwardVector, FVector::BackwardVector };

	for (int i=0; i<6; i++)
	{
		terrain[i]->Init(&planetSettings, RootComponent, directions[i], terrainMaterial, waterMaterial);
	}
	SetupGravity();
	active = true;
	body->Init();
	body->SetMassOverrideInKg(GetFName(), planetSettings.radius, true);

	FMath::RandInit(planetSettings.seed.Size());
	if (FMath::RandRange(0, 101) > 200)
		GenerateRing();
	else
		GenerateSatellites();

	SetSateliteInitialVelocites();
}

void APlanet::DestroyPlanet()
{
	for (auto satellite : satelliteActors)
	{
		satellite->Destroy();
	}

	Destroy();
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

	vel = body->GetComponentVelocity();
	speed = vel.Size();
	satelliteRoot->SetWorldLocation(GetActorLocation());
	SimulateSateliteOrbits();
	SpinPlanet();
}

void APlanet::SetupGravity()
{
	//gravityField->radius = surfaceSettings->radius * gravityOffset;// *(surfaceSettings->radius * gravityOffset);
}

void APlanet::GenerateTerrain()
{
	for (auto &t : terrain)
	{
		t->ConstructQuadTree();
	}
}

void APlanet::GenerateSatellites()
{
	auto satelliteClass = Cast<AUniverseSettings>(GetWorldSettings())->satelliteTemplate;
	const int count = FMath::RandRange(1, 7);

	const FRotator rotator;
	const FActorSpawnParameters spawnParams;
	satelliteRoot->SetWorldLocation(GetActorLocation());
	for (int i = 0; i < count; i++)
	{
		//FName name("Satellite %i", i);
		//UCelestialBody* newSatellite = NewObject<UCelestialBody>(this, satelliteClass, name);
		//
		//if (!newSatellite)
		//	return;
		//newSatellite->RegisterComponent();
		//newSatellite->SetSimulatePhysics(true);
		//newSatellite->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));
		//
		//newSatellite->Init();
		//newSatellite->SetRelativeLocation(AUniverseSettings::GetRandomLocationOnSphere(planetSettings.radius * 5));
		//newSatellite->SetWorldScale3D(FVector::OneVector * 100000.f);
		//newSatellite->SetMassOverrideInKg(newSatellite->GetFName(), 100000, true);
		//satellites.Add(newSatellite);

		ASatellite* newSatellite = GetWorld()->SpawnActor<ASatellite>(satelliteClass, GetActorLocation() + AUniverseSettings::GetRandomLocationOnSphere(planetSettings.radius * 3), rotator , spawnParams);
		newSatellite->AttachToComponent(satelliteRoot, FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));
		satelliteActors.Add(newSatellite);
		satelliteBodies.Add(newSatellite->body);
	}
}

void APlanet::GenerateRing()
{
	auto satelliteClass = Cast<AUniverseSettings>(GetWorldSettings())->satelliteTemplate;

	for (int i=0; i<3000; i++)
	{
		FName name("Satellite %i", i);
		UCelestialBody* newSatellite = NewObject<UCelestialBody>(this, satelliteClass, name);

		if (!newSatellite)
			return;
		newSatellite->RegisterComponent();
		newSatellite->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true));
		newSatellite->SetSimulatePhysics(true);
		newSatellite->SetRelativeLocation(AUniverseSettings::GetRandomLocationOnCircle(planetSettings.radius * 5));
		newSatellite->SetWorldScale3D(FVector::OneVector * 1000.f);
		satellites.Add(newSatellite);
	}
}

void APlanet::SimulateSateliteOrbits()
{
	for (auto satellite : satelliteBodies)
	{
		//satellite->MoveWithParent(GetActorLocation());
		satellite->ApplyForceBetween(body);
		//satellite->AddForce(FVector::ForwardVector * 100000000000.f);
	}
}

void APlanet::SetSateliteInitialVelocites()
{
	for (auto satellite : satelliteBodies)
	{
		//satellite->body->SetAllPhysicsLinearVelocity(FVector::ZeroVector);
		satellite->SetInitialVelocity(body);
	}
}

void APlanet::SpinPlanet()
{
	float delta = GetWorld()->GetDeltaSeconds();
	FQuat rotation = FQuat(FRotator(0.f, delta * rotationSpeed, 0.f));
	AddActorLocalRotation(rotation);
}
