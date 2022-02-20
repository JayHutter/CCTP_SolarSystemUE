// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "CelestialBody.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CCTP_SOLARSYSTEMUE_API UCelestialBody : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCelestialBody();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void ApplyForce(float force, FVector direction, float timestep=1);
	void ApplyForceBetween(UCelestialBody* otherBody, float timestep = 1);

	FVector velocity;
	UPROPERTY(EditAnywhere)
	float mass;
};
