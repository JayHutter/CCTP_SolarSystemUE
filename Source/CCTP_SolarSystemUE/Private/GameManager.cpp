// Fill out your copyright notice in the Description page of Project Settings.


#include "GameManager.h"
#include "UniverseSettings.h"

// Sets default values
AGameManager::AGameManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGameManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGameManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (currentTimeScale != timeScale)
	{
		Cast<AUniverseSettings>(GetWorldSettings())->SetTimeDilation(timeScale);

		UE_LOG(LogTemp, Log, TEXT("Current Time Dilation %F"), Cast<AUniverseSettings>(GetWorldSettings())->TimeDilation);
		currentTimeScale = timeScale;
	}
}

