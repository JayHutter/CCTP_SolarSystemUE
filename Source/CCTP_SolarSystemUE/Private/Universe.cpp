// Fill out your copyright notice in the Description page of Project Settings.


#include "Universe.h"
#include "UniverseSettings.h"

// Sets default values
AUniverse::AUniverse()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	sceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = sceneComponent;
}

// Called when the game starts or when spawned
void AUniverse::BeginPlay()
{
	Super::BeginPlay();

	AUniverseSettings* settings = Cast<AUniverseSettings>(GetWorldSettings());
	if (settings->universe == nullptr)
		settings->universe = this;
	else
		Destroy();
}

// Called every frame
void AUniverse::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AUniverse::UpdatePostion(FVector playerPosition)
{
	SetActorLocation(playerPosition * -1.f);
}

void AUniverse::SetCentrePoint(FVector centrePoint)
{
	FVector newPos = GetActorLocation() - centrePoint;
	SetActorLocation(newPos);
}

