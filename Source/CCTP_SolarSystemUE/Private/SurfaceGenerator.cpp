// Fill out your copyright notice in the Description page of Project Settings.


#include "SurfaceGenerator.h"

float SurfaceGenerator::ApplyNoise(FVector vertex, FPlanetSettings* settings)
{
	float noise = 0;

	if (settings->noiseSettings.Num() == 0)
		return 1;

	vertex += settings->seed;

	for (int i=0; i<settings->noiseSettings.Num(); i++)
	{
		FNoiseSettings noiseSettings = settings->noiseSettings[i];

		if (!noiseSettings.enabled)
			continue;

		switch (settings->noiseSettings[i].noiseType)
		{
			case NoiseType::SIMPLE:
				noise += SimpleNoise(vertex, noiseSettings.simpleNoiseSettings);
				break;
			case NoiseType::RIGID:
				noise += RigidNoise(vertex, noiseSettings.rigidNoiseSettings);
				break;
		}
	}

	return noise;
}

float SurfaceGenerator::SimpleNoise(FVector coord, FSimpleNoise settings)
{
	float noise = 0;
	float amplitude = settings.amplitude;
	float frequency = settings.baseRoughness;

	for (int i = 0; i < settings.noiseLayers; i++)
	{
		float v = FMath::PerlinNoise3D(coord * frequency * settings.frequencyMultiplier);
		//noise += (v + 1) * 0.5f * amplitude;
		noise += v  * amplitude;
		frequency *= settings.baseRoughness;
		amplitude *= settings.persistence;
	}

	noise = FMath::Max<float>(0, noise - settings.limiter);
	return noise * settings.strength;
}

float SurfaceGenerator::RigidNoise(FVector coord, FRigidNoise settings)
{
	float noise = 0;
	float amplitude = settings.amplitude;
	float weight = 1;
	float frequency = settings.baseRoughness;

	for (int i = 0; i < settings.noiseLayers; i++)
	{
		float v = 1 - FMath::PerlinNoise3D(coord * frequency * settings.frequencyMultiplier);

		v *= v;
		v *= weight;
		weight = FMath::Clamp<float>(0, 1.f, v * settings.weightMultiplier);

		noise += v * amplitude;

		frequency *= settings.roughness;
		amplitude *= settings.persistence;
	}

	noise = FMath::Max<float>(0, noise - settings.limiter);
	return noise * settings.strength;
}
