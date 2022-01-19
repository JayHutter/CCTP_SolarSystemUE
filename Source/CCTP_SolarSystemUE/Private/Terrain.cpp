// Fill out your copyright notice in the Description page of Project Settings.


#include "Terrain.h"

// Sets default values for this component's properties
UTerrain::UTerrain()
{
	mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Procedural Mesh"));
	//static ConstructorHelpers::FObjectFinder<UMaterial>MaterialObjectSurface(TEXT("Material'/Game/Materials/PlanetTest.PlanetTest'"));
	//static ConstructorHelpers::FObjectFinder<UMaterial>MaterialObjectWater(TEXT("Material'/Game/Materials/WaterTest.WaterTest'"));
	chunks.Empty();
}

void UTerrain::Init(USurfaceSettings* _settings, FVector _localUp)//, int32 _resolution)
{
	//resolution = _resolution;
	localUp = _localUp;

	mesh->SetRelativeLocation(FVector(0.f, 0.f, 0.f));

	if (_settings)
		settings = _settings;

	axisA = FVector(localUp.Z, localUp.X, localUp.Y);
	axisB = FVector::CrossProduct(localUp, axisA);

	faceLocation = localUp * settings->radius;
}

// Called when the game starts
void UTerrain::BeginPlay()
{
	Super::BeginPlay();	
}

void UTerrain::BuildMesh(int resolution)
{
	ResetData();

	if (currentActive == active && resolution == currentResolution)
		return;

	currentActive = active;
	currentResolution = resolution;

	if (!active)
	{
		mesh->ClearAllMeshSections();
		return;
	}

	//if (surfaceMaterial != NULL && !water)
	//{
	//	UMaterialInstanceDynamic* surfaceMaterialDyn = UMaterialInstanceDynamic::Create(surfaceMaterial, GetOwner());
	//	mesh->SetMaterial(0, surfaceMaterialDyn);
	//}
	//
	//if (waterMaterial != NULL && water)
	//{
	//	UMaterialInstanceDynamic* waterMaterialDyn = UMaterialInstanceDynamic::Create(waterMaterial, GetOwner());
	//	mesh->SetMaterial(0, waterMaterialDyn);
	//}

	int triCount = (resolution - 1) * (resolution - 1) * 6;
	int32 triIndex = 0;

	FProcMeshTangent tangent = FProcMeshTangent(0.f, 1.f, 0.f);

	for (int32 y=0; y<resolution; y++)
	{
		for (int32 x=0; x<resolution; x++)
		{
			int32 i = x + y * resolution;
			FVector2D percent = (FVector2D(x, y) / (resolution - 1));// *chunkSize + (FVector2D(0, 1) * chunkSize);
			FVector pointOnUnitCube = localUp +
				(percent.Y - 0.5f) * 2 * axisA + (percent.X - 0.5f) * 2 * axisB;
			FVector pointOnUnitSphere = CubeToSphere(pointOnUnitCube);

			float elevation = 0;

			//if (!water)
			//	elevation = SurfaceGenerator::ApplyNoise(pointOnUnitSphere, settings);
			//else
			//	elevation = .5f;
			elevation = SurfaceGenerator::ApplyNoise(pointOnUnitSphere, settings);
			FVector vertex = pointOnUnitSphere * settings->radius * (1 + elevation);
			vertices.Add(vertex);

			normals.Add(CalculateNormal(vertex));
			tangents.Add(tangent);
			vertexColors.Add(FColor::Green);
			uvs.Add(percent);

			if (x!= resolution-1 && y!= resolution-1)
			{
				triangles.Add(i);
				triangles.Add(i + resolution + 1);
				triangles.Add(i + resolution);

				triangles.Add(i);
				triangles.Add(i + 1);
				triangles.Add(i + resolution + 1);
				triIndex += 6;
			}
		}
	}

	mesh->ClearAllMeshSections();
	mesh->CreateMeshSection(0, vertices, triangles, normals, uvs, vertexColors, tangents, true);
}

void UTerrain::DetermineVisibility(FVector planetPos, FVector camPos)
{
	const float planetDistance = FVector::Distance(planetPos, camPos);
	const float faceDistance = FVector::Distance(planetPos + faceLocation, camPos);

	UE_LOG(LogTemp, Log, TEXT("Planet Distance: %f"), planetDistance);
	UE_LOG(LogTemp, Log, TEXT("Face Distance: %f"), faceDistance);

	active = planetDistance > faceDistance;
}

void UTerrain::ResetData()
{
	vertices.Reset();
	normals.Reset();
	triangles.Reset();
	uvs.Reset();
	vertexColors.Reset();
	tangents.Reset();
}

void UTerrain::CreateChunks()
{
	for (int x=0; x<2; x++)
	{
		for (int y=0; y<2; y++)
		{
			chunks[x + y];
		}
	}
}

FVector UTerrain::CubeToSphere(FVector vertexPos)
{
	float x = vertexPos.X;
	float y = vertexPos.Y;
	float z = vertexPos.Z;

	float x2 = x * x;
	float y2 = y * y;
	float z2 = z * z;

	x = x * sqrt(1 - (y2 / 2.f) - (z2 / 2.f) + ((y2 * z2) / 3.f));
	y = y * sqrt(1 - (z2 / 2.f) - (x2 / 2.f) + ((z2 * x2) / 3.f));
	z = z * sqrt(1 - (x2 / 2.f) - (y2 / 2.f) + ((x2 * y2) / 3.f));

	return FVector(x, y, z);
}

FVector UTerrain::CalculateNormal(FVector vertexPos)
{
	FVector dir = vertexPos - mesh->GetRelativeLocation();
	dir.Normalize();
	return dir.GetSafeNormal();
}