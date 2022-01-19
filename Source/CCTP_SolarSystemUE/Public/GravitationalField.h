// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GravitationalField.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CCTP_SOLARSYSTEMUE_API UGravitationalField : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGravitationalField();
	~UGravitationalField();
	UPROPERTY(EditAnywhere)
	float radius = 1000;

	UPROPERTY(EditAnywhere)
	float gravityAcceleration = 100;

	UPROPERTY(EditAnywhere)
	bool drawDebug;

	UPROPERTY(EditAnywhere)
	bool outputLog;

	UPROPERTY(VisibleAnywhere)
	FVector centre;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	void ApplyForceToActors();
	bool ApplyForceToCharacter(AActor* actor);
	FVector CalculateForce(FVector actorPos, float mass);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	TArray<TEnumAsByte<EObjectTypeQuery>> physicsChannel;
	TArray<AActor*> ignoreActors;
	UClass* searchClass;
};
