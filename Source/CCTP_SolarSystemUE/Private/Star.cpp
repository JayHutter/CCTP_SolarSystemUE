// Fill out your copyright notice in the Description page of Project Settings.


#include "Star.h"

// Sets default values
AStar::AStar()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Surface"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh>SphereMeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
	mesh->SetStaticMesh(SphereMeshAsset.Object);

	RootComponent = mesh;

	light = CreateDefaultSubobject<UPointLightComponent>(TEXT("Light"));
	light->AttachToComponent(mesh, FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));
	color = FLinearColor(0, 0, 1);
}

void AStar::Init(float radius, UMaterialInterface* material)
{
	mesh->SetWorldScale3D(FVector(radius, radius, radius));
	mesh->SetMaterial(0, material);
	materialInstance = UMaterialInstanceDynamic::Create(mesh->GetMaterial(0), this);
}

// Called every frame
void AStar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	

	age += GetWorld()->DeltaTimeSeconds;
	temp -= GetWorld()->DeltaTimeSeconds * 1000;
	
	float r = color.R;
	float g = color.G;
	float b = color.B;

	r += GetWorld()->DeltaTimeSeconds;

	color = FLinearColor::MakeFromColorTemperature(temp);

	if (materialInstance)
	{
		materialInstance->SetVectorParameterValue(FName(TEXT("Color")), color);
		mesh->SetMaterial(0, materialInstance);
	}
}