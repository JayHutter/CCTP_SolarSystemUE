// Fill out your copyright notice in the Description page of Project Settings.


#include "CelestialBody.h"
#include "UniverseSettings.h"

// Sets default values for this component's properties
UCelestialBody::UCelestialBody()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Surface"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh>SphereMeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
	SetStaticMesh(SphereMeshAsset.Object);
	SetupCollisions();
}


// Called when the game starts
void UCelestialBody::BeginPlay()
{
	Super::BeginPlay();

	//auto settings = Cast<AUniverseSettings>(GetWorld()->GetWorldSettings());
	//g = settings->gravitationalConstant;
	//massScale = settings->massScale;
}

void UCelestialBody::SetupCollisions()
{
	SetSimulatePhysics(true);
	SetCollisionProfileName(TEXT("NoCollision"));
	SetCollisionObjectType(ECollisionChannel::ECC_PhysicsBody);
	SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SetLinearDamping(0);
}


void UCelestialBody::Init(float gravityConstant, float massMultiplier)
{
	this->g = gravityConstant;
	this->massScale = massMultiplier;
	SetSimulatePhysics(true);
}

void UCelestialBody::Init()
{
	auto settings = Cast<AUniverseSettings>(GetWorld()->GetWorldSettings());
	g = settings->gravitationalConstant;
	massScale = settings->massScale;
}

// Called every frame
void UCelestialBody::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	FVector vel = GetPhysicsLinearVelocity();
	//UE_LOG(LogTemp, Log, TEXT("O: %s, F: %f, %f, %f"), *GetOwner()->GetName(), vel.X, vel.Y, vel.Z);
}

void UCelestialBody::ApplyForceBetween(UCelestialBody* otherBody)
{
	FVector posA = GetComponentLocation();
	FVector posB = otherBody->GetComponentLocation();
	float rSquared = FMath::Square(FVector::Distance(posA, posB) / scale);
	float massA = CalculateMass(GetFName()) / massScale;
	float massB = otherBody->CalculateMass(otherBody->GetFName()) / massScale;

	if (rSquared == 0)
		return;

	float magnitude = massA * massB / rSquared;;

	FVector forceA = (posB - posA).GetSafeNormal() * magnitude * g;
	//UE_LOG(LogTemp, Log, TEXT("O: %s, F: %f, %f, %f"), *GetOwner()->GetName(), forceA.X, forceA.Y, forceA.Z);
	AddForce(forceA * scale, GetFName(), false);
}

void UCelestialBody::SetInitialVelocity(UCelestialBody* otherBody)
{
	float otherMass = otherBody->CalculateMass(otherBody->GetFName()) / massScale;
	FVector posA = GetComponentLocation();
	FVector posB = otherBody->GetComponentLocation();
	float r = FVector::Distance(posA, posB) / scale;

	FVector aToB = posB - posA;
	FRotator aim = (posB - posA).Rotation();
	SetWorldRotation(aim);

	if (r == 0)
		return;

	FVector v = GetRightVector() * FMath::Sqrt((g * otherMass) / r);
	
	SetAllPhysicsLinearVelocity(v * scale, true);
}


FVector UCelestialBody::TeleportTo(FVector startPos)
{
	FVector direction = startPos - GetComponentLocation();
	direction.Normalize();

	return GetComponentLocation() + direction * offsetDistance;
}
