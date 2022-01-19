// Fill out your copyright notice in the Description page of Project Settings.


#include "GravitationalField.h"
#include "DrawDebugHelpers.h"
#include "Kismet\KismetSystemLibrary.h"
#include <Runtime/Engine/Classes/GameFramework/CharacterMovementComponent.h>

// Sets default values for this component's properties
UGravitationalField::UGravitationalField()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

UGravitationalField::~UGravitationalField()
{

}

// Called when the game starts
void UGravitationalField::BeginPlay()
{
	Super::BeginPlay();

	//physicsChannel.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PhysicsBody));
	ignoreActors.Init(GetOwner(), 1);
	searchClass = NULL;
}


// Called every frame
void UGravitationalField::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	centre = GetOwner()->GetActorLocation();
	ApplyForceToActors();
}

void UGravitationalField::ApplyForceToActors()
{
	if (drawDebug)
		DrawDebugSphere(GetWorld(), centre, radius, 50, FColor(150, 0, 0), true,-1,  1, radius/200);	
	
	TArray<AActor*> outActors;

	UKismetSystemLibrary::SphereOverlapActors(GetWorld(), centre, radius, physicsChannel, searchClass, ignoreActors, outActors);

	for (AActor* actor : outActors)
	{
		if (outputLog)
			UE_LOG(LogTemp, Log, TEXT("OverlappedActor: %s"), *actor->GetName());

		TArray<UStaticMeshComponent*> Components;
		actor->GetComponents<UStaticMeshComponent>(Components);
		for (int32 i = 0; i < Components.Num(); i++)
		{
			UStaticMeshComponent* staticMeshComponent = Components[i];

			if (!staticMeshComponent || ApplyForceToCharacter(actor))
				break;

			if (staticMeshComponent->IsSimulatingPhysics() && staticMeshComponent->IsGravityEnabled())
			{
				if (outputLog)
					UE_LOG(LogTemp, Log, TEXT("Applying Force To Object"));

				FVector force = CalculateForce(actor->GetActorLocation(), staticMeshComponent->GetMass());
				staticMeshComponent->AddForce(force);
			}
		}
	}
}

bool UGravitationalField::ApplyForceToCharacter(AActor* actor)
{
	TArray<UCharacterMovementComponent*> Components;
	actor->GetComponents<UCharacterMovementComponent>(Components);

	bool applied = false;

	for (int32 i = 0; i < Components.Num(); i++)
	{
		UCharacterMovementComponent* characterMovement = Components[i];
		if (!characterMovement)
			return false;

		applied = true;
		FVector force = CalculateForce(actor->GetActorLocation(), characterMovement->Mass);
		if (outputLog)
			UE_LOG(LogTemp, Log, TEXT("Applying Force To Character"));
		characterMovement->AddForce(force);
	}

	return applied;
}

FVector UGravitationalField::CalculateForce(FVector actorPos, float mass)
{
	FVector force = centre - actorPos;
	force.Normalize();
	force *= gravityAcceleration * mass;

	return force;
}