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

	SetSimulatePhysics(true);
	SetCollisionProfileName(TEXT("PhysicsActor"));
	SetLinearDamping(0);
	
}


// Called when the game starts
void UCelestialBody::BeginPlay()
{
	Super::BeginPlay();

	auto settings = Cast<AUniverseSettings>(GetWorld()->GetWorldSettings());
	g = settings->gravitationalConstant;
	massScale = settings->massScale;
}


// Called every frame
void UCelestialBody::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UCelestialBody::ApplyForceBetween(UCelestialBody* otherBody)
{
	FVector posA = GetComponentLocation();
	FVector posB = otherBody->GetComponentLocation();
	float rSquared = FMath::Square(FVector::Distance(posA, posB));
	float massA = CalculateMass(GetFName()) / massScale;
	float massB = otherBody->CalculateMass(otherBody->GetFName()) / massScale;
	float magnitude = massA * massB / rSquared;;

	FVector forceA = (posB - posA).GetSafeNormal() * magnitude * g;

	AddForce(forceA, GetFName(), false);
}

void UCelestialBody::SetInitialVelocity(UCelestialBody* otherBody)
{
	float otherMass = otherBody->CalculateMass(otherBody->GetFName()) / massScale;
	FVector posA = GetComponentLocation();
	FVector posB = otherBody->GetComponentLocation();
	float r = FVector::Distance(posA, posB);

	FVector aToB = posB - posA;
	FRotator aim = (posB - posA).Rotation();
	SetWorldRotation(aim);

	FVector v = GetRightVector() * FMath::Sqrt((g * otherMass) / r);
	SetAllPhysicsLinearVelocity(v, true);
}
