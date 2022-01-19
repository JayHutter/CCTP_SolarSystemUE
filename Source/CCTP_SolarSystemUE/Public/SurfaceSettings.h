// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NoiseSettings.h"
#include "SurfaceSettings.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CCTP_SOLARSYSTEMUE_API USurfaceSettings : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USurfaceSettings() {};

	UPROPERTY(EditAnywhere)
	float radius = 500.f;

	UPROPERTY(EditAnywhere)
	TArray<FNoiseSettings> noiseSettings;
};
