// Fill out your copyright notice in the Description page of Project Settings.


#include "AimAtCamera.h"

// Sets default values for this component's properties
UAimAtCamera::UAimAtCamera()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAimAtCamera::BeginPlay()
{
	Super::BeginPlay();

	if (UWorld* World = GetWorld())
		playerCamera = World->GetFirstPlayerController()->PlayerCameraManager;

	parent = GetOwner();
}


// Called every frame
void UAimAtCamera::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FRotator aim = (playerCamera->GetCameraLocation() - parent->GetActorLocation()).Rotation();
	parent->SetActorRotation(aim, ETeleportType::None);
}

