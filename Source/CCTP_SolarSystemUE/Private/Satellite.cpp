// Fill out your copyright notice in the Description page of Project Settings.


#include "Satellite.h"

// Sets default values
ASatellite::ASatellite()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	body = CreateDefaultSubobject<UCelestialBody>(TEXT("Body"));
	body->SetWorldScale3D(FVector::OneVector * 100000.f);
	
	RootComponent = body;
}

// Called when the game starts or when spawned
void ASatellite::BeginPlay()
{
	Super::BeginPlay();
	body->Init(10000000000000.f, 1);
	body->SetMassOverrideInKg(GetFName(), 10000000.f, true);
}

// Called every frame
void ASatellite::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//Move the satellite by the same amount as the planet has moved since last update
void ASatellite::MoveWithParent(FVector currentLocation)
{
	FVector movement = currentLocation - prevLocation;
	AddActorWorldOffset(movement);

	prevLocation = currentLocation;
}

