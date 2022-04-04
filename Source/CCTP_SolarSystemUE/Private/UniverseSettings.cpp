// Fill out your copyright notice in the Description page of Project Settings.


#include "UniverseSettings.h"

FVector AUniverseSettings::GetRandomLocationInCircle(float radius, float minimumDistance)
{
	float r = radius * FMath::Sqrt(FMath::RandRange(0.f, 1.f));
	float theta = FMath::RandRange(0.f, 1.f) * 2.f * PI;

	r += minimumDistance;

	float x = r * FMath::Cos(theta);
	float y = r * FMath::Sin(theta);
	float z = r * FMath::Sin(theta);

	return FVector(x, y, z);
}

FVector AUniverseSettings::GetRandomLocationOnCircle(float radius)
{
	float x = FMath::RandRange(-5.f, 5.f);
	float y = FMath::RandRange(-5.f, 5.f);
	FVector location = FVector(x, y, 0);
	location.Normalize();

	return location * radius;
}

FVector AUniverseSettings::GetRandomLocationOnSphere(float radius)
{
	float x = FMath::RandRange(-5.f, 5.f);
	float y = FMath::RandRange(-5.f, 5.f);
	float z = FMath::RandRange(-5.f, 5.f);
	FVector location = FVector(x, y, z);
	location.Normalize();

	return location * radius;
}
