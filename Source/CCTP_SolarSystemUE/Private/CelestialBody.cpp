// Fill out your copyright notice in the Description page of Project Settings.


#include "CelestialBody.h"

// Sets default values for this component's properties
UCelestialBody::UCelestialBody()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCelestialBody::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UCelestialBody::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UCelestialBody::ApplyForce(float force, FVector direction, float timestep)
{
	//a = f/m (f = ma)
	//s = vt + .5a(t^2)
	//s = distance, v = initial velocity on frame, t = timestep

	float acceleration = force / mass;

	float distance = velocity.Size() * timestep + (0.5f * acceleration * timestep * timestep);

	UE_LOG(LogTemp, Log, TEXT("FORCE %f"), force);
	FVector loc = distance * direction + GetComponentLocation();
	velocity += force * direction;
	SetWorldLocation(loc);
}

void UCelestialBody::ApplyForceBetween(UCelestialBody* otherBody, float timestep)
{
	FVector dir = (otherBody->GetComponentLocation() - GetComponentLocation()).GetSafeNormal();
	float rSquared = FMath::Square(FVector::Distance(GetComponentLocation(), otherBody->GetComponentLocation()));

	UE_LOG(LogTemp, Log, TEXT("R2 %f"), rSquared);

	float acceleration = otherBody->mass / rSquared;
	float distance = velocity.Size() * timestep + (0.5f * acceleration * timestep * timestep);
	FVector loc = distance * dir + GetComponentLocation();
	//velocity += mass * acceleration * dir;
	SetWorldLocation(loc);
}

