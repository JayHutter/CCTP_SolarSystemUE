// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NoiseSettings.generated.h"

UENUM()
enum NoiseType
{
	SIMPLE UMETA(DisplayName = "Simple Noise"),
	RIGID UMETA(DisplayName = "Rigid Noise")
};

USTRUCT()
struct FSimpleNoise
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float baseRoughness = 1;

	UPROPERTY(EditAnywhere)
	float roughness = 2.f;

	UPROPERTY(EditAnywhere)
	float strength = 1.f;

	UPROPERTY(EditAnywhere)
	FVector noiseOffset;

	UPROPERTY(EditAnywhere)
	int noiseLayers = 3;

	UPROPERTY(EditAnywhere)
	float persistence = 0.5f;

	UPROPERTY(EditAnywhere)
	float limiter = 0;
};

USTRUCT()
struct FRigidNoise : public FSimpleNoise
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Rigid Noise")
	float weightMultiplier = 1.f;
};

USTRUCT()
struct FNoiseSettings
{
	GENERATED_BODY()

	FNoiseSettings() {};

	UPROPERTY(EditAnywhere)
	bool enabled = true;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<NoiseType> noiseType = NoiseType::SIMPLE;

	UPROPERTY(EditAnywhere)
	FSimpleNoise simpleNoiseSettings;
	UPROPERTY(EditAnywhere)
	FRigidNoise rigidNoiseSettings;
};


