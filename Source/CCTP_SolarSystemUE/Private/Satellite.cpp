// Fill out your copyright notice in the Description page of Project Settings.


#include "Satellite.h"

// Sets default values
ASatellite::ASatellite()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASatellite::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASatellite::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

