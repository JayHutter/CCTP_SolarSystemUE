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

	float g;
	float massScale;
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void ApplyForceBetween(UCelestialBody* otherBody);
	void SetInitialVelocity(UCelestialBody* otherBody);
};
