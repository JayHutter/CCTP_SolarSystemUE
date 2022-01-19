// Fill ut your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProceduralMeshComponent.h"
#include "SurfaceGenerator.h"
#include "Terrain.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CCTP_SOLARSYSTEMUE_API UTerrain : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTerrain();
	void Init(USurfaceSettings* _settings, FVector _localUp = FVector(0.f, 0.f, 1.f));//, int32 _resolution = 5);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	UPROPERTY(VisibleAnywhere)
	UProceduralMeshComponent* mesh;

	//UPROPERTY(EditAnywhere)
	//int32 resolution;

	UPROPERTY(VisibleAnywhere)
	FVector localUp;
	UPROPERTY(VisibleAnywhere)
	FVector axisA;
	UPROPERTY(VisibleAnywhere)
	FVector axisB;

	void BuildMesh(int resolution=32);
	void DetermineVisibility(FVector planetPos, FVector camPos);
	void ResetData();
	void CreateChunks();

	UMaterial* surfaceMaterial;
	UMaterial* waterMaterial;

	TArray<UTerrain*> chunks;
	int detailLevel = 1;

	FVector GetFaceLocation() const { return faceLocation; }
	bool active = true;
private:
	TArray<FVector> vertices;
	TArray<FVector> normals;
	TArray<int32> triangles;
	TArray<FVector2D> uvs;
	TArray<FColor> vertexColors;
	TArray<FProcMeshTangent> tangents;

	FVector faceLocation;

	FVector CubeToSphere(FVector vertexPos);
	FVector CalculateNormal(FVector vertexPos);
	USurfaceSettings* settings;
	int currentResolution;
	bool currentActive = false;
};
