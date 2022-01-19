// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleMeshGenerator.h"
#include "Engine.h"
#include "Engine/Private/VectorField.h"

// Sets default values
ASimpleMeshGenerator::ASimpleMeshGenerator()
{
	thisScene = CreateDefaultSubobject<USceneComponent>(TEXT("ROOT"));
	RootComponent = thisScene;

	procMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Procedural Mesh"));
	procMesh->SetupAttachment(RootComponent);
}

void ASimpleMeshGenerator::PostActorCreated()
{
	Super::PostActorCreated();
	GenerateMesh();
}

void ASimpleMeshGenerator::PostLoad()
{
	Super::PostLoad();
	GenerateMesh();
}

void ASimpleMeshGenerator::GenerateMesh()
{
	//Reset all mesh Variables to allow them to be updated
	vertices.Reset();
	normals.Reset();
	triangles.Reset();
	uvs.Reset();
	vertexColors.Reset();
	tangents.Reset();

	int32 indexBuffer = 0;
	FVector definedShape[3];
	FProcMeshTangent tangentSetup;

	definedShape[0] = FVector(cubeRadius.X, cubeRadius.Y, cubeRadius.Z);
	definedShape[1] = FVector(cubeRadius.X, cubeRadius.Y, -cubeRadius.Z);
	definedShape[2] = FVector(cubeRadius.X, -cubeRadius.Y, -cubeRadius.Z);

	tangentSetup = FProcMeshTangent(0.f, 1.f, 0.f);
	AddTriangleMesh(definedShape[0], definedShape[1], definedShape[2], indexBuffer, tangentSetup);

	procMesh->CreateMeshSection(0, vertices, triangles, normals, uvs, vertexColors, tangents, true);
}

void ASimpleMeshGenerator::AddTriangleMesh(FVector topLeft, FVector bottomLeft, FVector bottomRight, int32& triIndex, FProcMeshTangent tangent)
{
	int32 point1 = triIndex++;
	int32 point2 = triIndex++;
	int32 point3 = triIndex++;

	vertices.Add(topLeft);
	vertices.Add(bottomLeft);
	vertices.Add(bottomRight);

	triangles.Add(point1);
	triangles.Add(point2);
	triangles.Add(point3);

	FVector thisNorm = FVector::CrossProduct(topLeft, bottomRight).GetSafeNormal();

	for (int i=0; i<3; i++)
	{
		normals.Add((thisNorm));
		tangents.Add(tangent);
		vertexColors.Add(FColor::Red);
	}

	uvs.Add(FVector2D(0.f, 1.f));
	uvs.Add(FVector2D(0.f, 0.f));
	uvs.Add(FVector2D(1.f, 0.f));
}

