// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Terrain.h"
#include "GravitationalField.h"
#include "Planet.generated.h"

UCLASS()
class CCTP_SOLARSYSTEMUE_API APlanet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlanet();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//UPROPERTY(EditAnywhere)
	//bool basicPlanet;

	//UPROPERTY(EditAnywhere)
	//TArray<FNoiseSettings> noiseSettings;

	UPROPERTY(EditAnywhere)
	USurfaceSettings* surfaceSettings;

	UStaticMeshComponent* simpleMesh;

	UPROPERTY(VisibleAnywhere, Instanced)
	TArray<UTerrain*> terrain;

	UPROPERTY(EditAnywhere)
	UGravitationalField* gravityField;

	UPROPERTY(EditAnywhere)
	float gravityOffset = 50;

	void ManageLOD();
	int detailLevel = -1;

	void Init(float radius, TArray<FNoiseSettings> noiseSettings, int chunkResolution=8, UMaterialInterface* terrainMaterial = nullptr, UMaterialInterface* waterMaterial = nullptr);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void SetupGravity();
	void GenerateTerrain();

	bool active = false;
	int currentRes = 16;
	APlayerCameraManager* playerCamera;
	float timer = 0;
	UMaterialInterface* planetMaterial;
};
