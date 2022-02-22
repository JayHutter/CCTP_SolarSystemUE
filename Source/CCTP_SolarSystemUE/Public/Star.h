// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PointLightComponent.h"
#include "GameFramework/Actor.h"
#include "CelestialBody.h"
#include "Star.generated.h"

UCLASS()
class CCTP_SOLARSYSTEMUE_API AStar : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AStar();

	void Init(float radius, UMaterialInterface* material);

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UStaticMeshComponent* mesh;
	UMaterialInstanceDynamic* materialInstance;
	UPointLightComponent* light;
	UPROPERTY(VisibleAnywhere)
	FLinearColor color;
	UPROPERTY(EditAnywhere)
	UCelestialBody* body;

	float age = 0;
	float temp = 50000;
};
