// Fill ut your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProceduralMeshComponent.h"
#include "SurfaceGenerator.h"
#include "SurfaceSettings.h"
#include "Terrain.generated.h"

USTRUCT()
struct FTriangleData
{
	GENERATED_BODY()

	TArray<FVector> vertices;
	TArray<FVector> normals;
	TArray<int32> triangles;
	TArray<FVector2D> uvs;
	TArray<FColor> vertexColors;
	TArray<FProcMeshTangent> tangents;
};

class CCTP_SOLARSYSTEMUE_API Chunk
{
public:
	TArray<Chunk*> children;
	Chunk* parent;
	FVector location;
	float scale;
	int detailLevel;
	FVector localUp;
	FVector axisA;
	FVector axisB;
	FPlanetSettings* planetSettings;
	AActor* parentPlanet;

	Chunk(Chunk* parent, FVector location, float scale, int detailLevel, FVector upVector, 
		FVector axisA, FVector axisB, FPlanetSettings* planetSettings, AActor* parentPlanet, FString id="0");

	bool GenerateChildren(FVector cameraLocation);
	TArray<Chunk*> GetVisibleChildren();
	FTriangleData CalcuateTriangles(int triangleOffset);
	void CalculateTerrainAndWaterTris(FTriangleData& terrainData, FTriangleData& waterData);
	void CreateTriangle(FTriangleData& triangleData, FVector vertexPos, FVector2D percent, int resolution, int triOffset, int x, int y, float elavation = 1);
	void SaveMinMaxValues(float elevation);

	TMap<int, float> detailDistances =
	{
		{0, 2.f},
		{1, 1.1f},
		{2, 1.05f},
		{3, 1.f},
		{4, .95f},
		{5, .9f},
		{6, .85f},
		{7, .8f}
	};
	const int maxLOD = 5;
	FString id;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CCTP_SOLARSYSTEMUE_API UTerrain : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTerrain();
	void Init(FPlanetSettings* settings, USceneComponent* rootComponent, FVector localUp = FVector(0.f, 0.f, 1.f), UMaterialInterface* terrainMaterial = nullptr, UMaterialInterface* waterMaterial = nullptr);

protected:
	// Called when the game starts
	//virtual void BeginPlay() override;

public:	
	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	//void BuildMesh(int resolution);
	void ConstructQuadTree();
	//void DetermineVisibility(FVector planetPos, FVector camPos);
	void ResetData();

	int detailLevel = -1;
	FVector GetFaceLocation() const { return faceLocation; }
	static FVector CubeToSphere(FVector vertexPos);
	static FVector CalculateNormal(FVector vertexPos);
	void SetColors();

private:
	int terrainResolution;
	UPROPERTY(VisibleAnywhere)
	UProceduralMeshComponent* mesh;
	UPROPERTY(VisibleAnywhere)
	UProceduralMeshComponent* water;

	UPROPERTY(VisibleAnywhere)
	FVector localUp;
	UPROPERTY(VisibleAnywhere)
	FVector axisA;
	UPROPERTY(VisibleAnywhere)
	FVector axisB;
	UPROPERTY(VisibleAnywhere)
	bool active = true;

	FVector planetSeed;
	FVector faceLocation;

	FPlanetSettings* planetSettings;

	int currentResolution;
	bool currentActive = false;
	FProcMeshTangent basicTangent;

	Chunk* rootChunk;
	//UMaterialInterface* material;

	FTriangleData terrainData;
	FTriangleData waterData;
	UMaterialInstanceDynamic* dynamicTerrainMat;

	const float reflectDistance = 500000000;
	float reflectiveness = 0;
};


