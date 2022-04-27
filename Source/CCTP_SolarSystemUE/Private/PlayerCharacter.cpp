// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "UniverseSettings.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	
	//RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	Collider = CreateDefaultSubobject<USphereComponent>("Collider");
	//Collider->SetupAttachment(RootComponent);
	RootComponent = Collider;

	MeshRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Body"));
	MeshRoot->SetupAttachment(RootComponent);
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	Mesh->SetupAttachment(MeshRoot);
	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!universe)
	{
		universe = Cast<AUniverseSettings>(GetWorldSettings())->universe;
		//AttachToActor(universe, FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));
	}
	FVector location = GetActorLocation();
	//UpdateRelativeLocation();
	if (location.Size() > maxLocationMagnitude)
	{
		//universe->SetCentrePoint(location);
		//SetActorLocation(FVector::ZeroVector);
	}
	else
	{
		if (attachedSystem)
		{
			FVector vel = attachedSystem->GetPhysicsLinearVelocity();
			UE_LOG(LogTemp, Log, TEXT("System Vel %f, %f, %f"), vel.X, vel.Y, vel.Z);
			SetActorLocation(attachedSystem->GetComponentLocation());
		}

		auto parent = GetParentComponent();
		if (parent)
		{
			FVector loc = parent->GetComponentLocation();
			UE_LOG(LogTemp, Log, TEXT("Pos %f, %f, %f"), loc.X, loc.Y, loc.Z);
			Collider->SetWorldLocationAndRotationNoPhysics(loc, FRotator::ZeroRotator);
		}

		Collider->AddForce(MoveDirection * moveSpeed, GetFName(), true);
		FVector vel = Collider->GetPhysicsLinearVelocity();

		if (vel.Size() > 0)
			MeshRoot->SetWorldRotation(vel.Rotation());
		else
		{
			FRotator Rotator = MeshRoot->GetComponentRotation();
			Rotator.Pitch = 0;
			Rotator.Roll = 0;

			MeshRoot->SetWorldRotation(Rotator);
		}
	}

	
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &APlayerCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &APlayerCharacter::LookUpAtRate);
}

void APlayerCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

//Was going to find the system the player was looking at and teleport them to it (like jumping to hyperspace)
//Too costly and inaccurate to use for travel between planets
void APlayerCharacter::IsLookingAtObject()
{
	const FRotator Rotation = Controller->GetControlRotation();
	const FVector Direction = FRotationMatrix(Rotation).GetUnitAxis(EAxis::X);

	FHitResult outHit;
	FVector startPoint = GetActorLocation() + Direction * 999999999999999999;
	FVector endPoint = startPoint + Direction * 999999999999999999;
	FCollisionQueryParams params;

	params.AddIgnoredComponent(Collider);

	//bool isHit = ActorLineTraceSingle(outHit, startPoint, endPoint, ECC_WorldDynamic, params);

	bool isHit = GetWorld()->LineTraceSingleByObjectType(
		outHit,
		startPoint,
		endPoint,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		params);

	if (isHit)
	{
		UE_LOG(LogTemp, Log, TEXT("Hit Somthing %fm away"), outHit.Distance);

		UCelestialBody* body = Cast<UCelestialBody>(outHit.GetComponent());

		if (body)
		{
			UE_LOG(LogTemp, Log, TEXT("Hit Celestial Body"));
		}
	}
}

//If the player is parented, update the relative location so that it moves with it
void APlayerCharacter::UpdateRelativeLocation()
{
	auto parentActor = GetParentActor();
	if (parentActor)
	{
		FVector parentLoc = parentActor->GetActorLocation();
		FVector relLoc = GetActorLocation() - parentLoc;

		SetActorLocation(parentLoc + relLoc);
	}
}

void APlayerCharacter::AttachToSystemBody(UCelestialBody* body)
{
	attachedSystem = body;
}

void APlayerCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr))
	{
		if (Value == 0)
		{
			Collider->SetAllPhysicsLinearVelocity(FVector::ZeroVector);
		}

		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();

		// get forward vector
		const FVector Direction = FRotationMatrix(Rotation).GetUnitAxis(EAxis::X);
		MoveDirection = Direction * Value;
		//Mesh->SetRelativeRotation(Rotation);
	}
}

void APlayerCharacter::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		if (moveSpeed <= 1)
			moveSpeed = 1;
		
		moveSpeed += Value * speedIncrement;

		//if (Value > 0)
		//	moveSpeed = maxSpeed;
		//else
		//	moveSpeed = minSpeed;
	}
}