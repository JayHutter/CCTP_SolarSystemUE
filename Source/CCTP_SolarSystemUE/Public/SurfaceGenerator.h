// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SurfaceSettings.h"
#include "NoiseSettings.h"

/**
 * 
 */
class CCTP_SOLARSYSTEMUE_API SurfaceGenerator
{
public:
	static float ApplyNoise(FVector vertex, FPlanetSettings* settings);

	static float SimpleNoise(FVector coord, FSimpleNoise settings, float scale);
	static float RigidNoise(FVector coord, FRigidNoise settings, float scale);
};
