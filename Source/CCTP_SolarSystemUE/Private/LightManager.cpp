// Fill out your copyright notice in the Description page of Project Settings.


#include "LightManager.h"

#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
ULightManager::ULightManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void ULightManager::BeginPlay()
{
	Super::BeginPlay();

	if (UWorld* World = GetWorld())
		playerCamera = World->GetFirstPlayerController()->PlayerCameraManager;

	parent = GetOwner();
}


// Called every frame
void ULightManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	TArray<AActor*> allStars;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AStar::StaticClass(), allStars);


	AActor* closest = allStars[0];
	float smallestDist = -1;
	for (auto star : allStars)
	{
		float dist = FVector::Distance(playerCamera->GetCameraLocation(), star->GetActorLocation());
		if (dist <= smallestDist || smallestDist < 0)
		{
			smallestDist = dist;
			closest = star;
		}
	}

	FRotator aim = (playerCamera->GetCameraLocation() - closest->GetActorLocation()).Rotation();
	parent->SetActorRotation(aim, ETeleportType::None);
}

