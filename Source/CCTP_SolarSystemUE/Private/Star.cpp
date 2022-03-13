// Fill out your copyright notice in the Description page of Project Settings.


#include "Star.h"

#include "Components/DirectionalLightComponent.h"

// Sets default values
AStar::AStar()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	body = CreateDefaultSubobject<UCelestialBody>(TEXT("Body"));
	RootComponent = body;

	mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Surface"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh>SphereMeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
	mesh->SetStaticMesh(SphereMeshAsset.Object);
	//mesh->AttachToComponent(body, FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));
	mesh->SetRelativeLocation(FVector::ZeroVector);
	mesh->SetGenerateOverlapEvents(false);

	light = CreateDefaultSubobject<UPointLightComponent>(TEXT("Light"));
	light->AttachToComponent(mesh, FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));
	color = FLinearColor(0, 0, 1);

	lightSource = CreateDefaultSubobject<UDirectionalLightComponent>("Light Source");
	lightSource->AttachToComponent(mesh, FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));
}

void AStar::Init(float radius, UMaterialInterface* material)
{
	mesh->SetWorldScale3D(FVector(radius, radius, radius));
	mesh->SetMaterial(0, material);
	materialInstance = UMaterialInstanceDynamic::Create(mesh->GetMaterial(0), this);
	body->SetMassOverrideInKg(GetFName(), radius * radius * radius, true);

	if (UWorld* World = GetWorld())
		playerCamera = World->GetFirstPlayerController()->PlayerCameraManager;
}

// Called every frame
void AStar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	age += GetWorld()->DeltaTimeSeconds;
	temp -= GetWorld()->DeltaTimeSeconds * 1000;

	color = FLinearColor::MakeFromColorTemperature(temp);
	//float newmass = body->CalculateMass(GetFName()) + GetWorld()->DeltaTimeSeconds * 1000000;
	//body->SetMassOverrideInKg(GetFName(), newmass, true);

	if (materialInstance)
	{
		materialInstance->SetVectorParameterValue(FName(TEXT("Color")), color);
		mesh->SetMaterial(0, materialInstance);
	}

	FRotator aim = (playerCamera->GetCameraLocation() - GetActorLocation()).Rotation();
	lightSource->SetWorldRotation(aim);
	//lightSource->SetLightColor(color);
}