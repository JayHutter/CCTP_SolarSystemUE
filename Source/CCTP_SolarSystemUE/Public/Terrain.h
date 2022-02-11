// Fill ut your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProceduralMeshComponent.h"
#include "SurfaceGenerator.h"
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
	//virtual void BeginPlay() override;

public:	
	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void BuildMesh(int resolution);
	void ConstructQuadTree();
	void DetermineVisibility(FVector planetPos, FVector camPos);
	void ResetData();

	int detailLevel = -1;
	FVector GetFaceLocation() const { return faceLocation; }
	static FVector CubeToSphere(FVector vertexPos);
	static FVector CalculateNormal(FVector vertexPos);

private:
	UPROPERTY(VisibleAnywhere)
	TArray<FVector> vertices;
	TArray<FVector> normals;
	UPROPERTY(VisibleAnywhere)
	TArray<int32> triangles;
	TArray<FVector2D> uvs;
	TArray<FColor> vertexColors;
	TArray<FProcMeshTangent> tangents;
	int terrainResolution;
	UPROPERTY(VisibleAnywhere)
	UProceduralMeshComponent* mesh;

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

	USurfaceSettings* surfaceSettings;
	int currentResolution;
	bool currentActive = false;
	FProcMeshTangent basicTangent;
};

class CCTP_SOLARSYSTEMUE_API Chunk
{
public:
	TArray<Chunk*> children;
	Chunk* parent;
	FVector location;
	FVector planetLocation;
	float scale;
	int detailLevel;
	FVector localUp;
	FVector axisA;
	FVector axisB;
	USurfaceSettings* surfaceSettings;

	Chunk(Chunk* parent, FVector location, FVector planetLocation, float scale, int detailLevel, 
		FVector localUp, FVector axisA, FVector axisB, USurfaceSettings* surfaceSettings);

	void GenerateChildren(FVector cameraLocation);
	TArray<Chunk*> GetVisibleChildren();
	FTriangleData CalcuateTriangles(int triangleOffset);

	TMap<int, float> detailDistances =
	{
		{0, 1000.f},
		{1, 8.f},
		{2, 4.f},
		{3, 2.f},
		{4, 1.f},
		{5, 0.5f}
	};
	const int maxLOD = 5;
};
