// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "SimpleMeshGenerator.generated.h"

UCLASS()
class CCTP_SOLARSYSTEMUE_API ASimpleMeshGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASimpleMeshGenerator();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector cubeRadius = FVector(100.f, 100.f, 100.f);

protected:
	void GenerateMesh();

	virtual void PostActorCreated() override;
	virtual void PostLoad() override;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* thisScene;

	UPROPERTY(VisibleAnywhere)
	UProceduralMeshComponent* procMesh;

private:
	void AddTriangleMesh(FVector topLeft, FVector bottomLeft, FVector bottomRight, int32& triIndex, FProcMeshTangent tangent);

	TArray<FVector> vertices;
	TArray<FVector> normals;
	TArray<int32> triangles;
	TArray<FVector2D> uvs;
	TArray<FColor> vertexColors;
	TArray<FProcMeshTangent> tangents;
};
