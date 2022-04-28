// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "CelestialBody.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CCTP_SOLARSYSTEMUE_API UCelestialBody : public UStaticMeshComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCelestialBody();
	UPROPERTY(VisibleAnywhere)
	float scale = 1.f;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	void SetupCollisions();

	float g;
	float massScale;

	float offsetDistance = 1000;
	FVector lastVelocity;
public:
	void Init(float gravityConstant, float massMultiplier);
	void Init();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void ApplyForceBetween(UCelestialBody* otherBody);
	void SetInitialVelocity(UCelestialBody* otherBody);

	FVector TeleportTo(FVector startPos);
	void PauseMotion();
	void ResumeMotion();
};
