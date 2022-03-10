// Fill out your copyright notice in the Description page of Project Settings.


#include "Terrain.h"

// Sets default values for this component's properties
UTerrain::UTerrain()
{
	mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Terrain Mesh"));
	water = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Water Mesh"));
	basicTangent = FProcMeshTangent(0.f, 1.f, 0.f);
	PrimaryComponentTick.bCanEverTick = true;
}

void UTerrain::Init(FPlanetSettings* settings, USceneComponent* rootComponent, FVector upVector, UMaterialInterface* terrainMaterial, UMaterialInterface* waterMaterial)
{
	//resolution = _resolution;
	localUp = upVector;
	this->planetSettings = settings;

	axisA = FVector(localUp.Z, localUp.X, localUp.Y);
	axisB = FVector::CrossProduct(localUp, axisA);

	faceLocation = localUp * planetSettings->radius;
	planetSeed = GetOwner()->GetActorLocation();

	mesh->AttachToComponent(rootComponent, FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));
	mesh->SetRelativeLocation(FVector::ZeroVector);
	water->AttachToComponent(rootComponent, FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));
	water->SetRelativeLocation(FVector::ZeroVector);

	if (terrainMaterial)
	{
		//mesh->SetMaterial(0, terrainMaterial);
		dynamicTerrainMat = UMaterialInstanceDynamic::Create(terrainMaterial, mesh);
		mesh->SetMaterial(0, dynamicTerrainMat);
	}

	if (waterMaterial)
		water->SetMaterial(0, waterMaterial);

	rootChunk = new Chunk(nullptr, localUp, 1.f, -1, 
		localUp, axisA, axisB, planetSettings, GetOwner(), "0");

	UMaterialInstanceDynamic* dynMat = UMaterialInstanceDynamic::Create(terrainMaterial, mesh);
	mesh->SetMaterial(0, dynMat);
}
/*
void UTerrain::BuildMesh(int resolution)
{
	//mesh->SetRelativeLocation(FVector::ZeroVector);
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

	int triCount = (resolution - 1) * (resolution - 1) * 6;
	int32 tri = 0;

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
			elevation = SurfaceGenerator::ApplyNoise(pointOnUnitSphere + planetSeed, surfaceSettings);
			FVector vertex = pointOnUnitSphere * surfaceSettings->radius * (1 + elevation);
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
				tri += 6;
			}
		}
	}

	mesh->ClearAllMeshSections();
	mesh->CreateMeshSection(0, vertices, triangles, normals, uvs, vertexColors, tangents, true);
	
	//mesh->SetRelativeLocation(FVector::ZeroVector);
}
*/

void UTerrain::ConstructQuadTree()
{
	//UE_LOG(LogTemp, Log, TEXT("TIME START: %fs"), GetWorld()->TimeSeconds);
	FVector camLocation = GetWorld()->GetFirstPlayerController()->PlayerCameraManager->GetCameraLocation();
	if (rootChunk->GenerateChildren(camLocation))
	{
		ResetData();
		mesh->ClearAllMeshSections();
		water->ClearAllMeshSections();

		for (auto&child : rootChunk->GetVisibleChildren())
		{
			child->CalculateTerrainAndWaterTris(terrainData, waterData);
		}

		mesh->CreateMeshSection(0, terrainData.vertices, terrainData.triangles, terrainData.normals, terrainData.uvs, terrainData.vertexColors, terrainData.tangents, true);
		water->CreateMeshSection(0, waterData.vertices, waterData.triangles, waterData.normals, waterData.uvs, waterData.vertexColors, waterData.tangents, false);
		SetColors();
		ResetData(); //Empty arrays to save mem
	}
	//UE_LOG(LogTemp, Log, TEXT("TIME END: %fs"), GetWorld()->TimeSeconds);
}

/*
void UTerrain::DetermineVisibility(FVector planetPos, FVector camPos)
{
	const float planetDistance = FVector::Distance(planetPos, camPos);
	const float faceDistance = FVector::Distance(planetPos + faceLocation, camPos);

	//UE_LOG(LogTemp, Log, TEXT("Planet Distance: %f"), planetDistance);
	//UE_LOG(LogTemp, Log, TEXT("Face Distance: %f"), faceDistance);

	active = true;// planetDistance > faceDistance;

	int resolution = 0;

	if (faceDistance >= 300000.f)
	{
		//UE_LOG(LogTemp, Log, TEXT("LOD 0"));
		resolution = 4;
		detailLevel = 0;
	}
	else if (faceDistance >= 150000.f)
	{
		//UE_LOG(LogTemp, Log, TEXT("LOD 0"));
		resolution = 16;
		detailLevel = 0;
	}
	else if (faceDistance >= 50000.f && faceDistance < 150000.f)
	{
		//UE_LOG(LogTemp, Log, TEXT("LOD 1"));
		resolution = 64;
		detailLevel = 1;
	}
	else if (faceDistance < 50000.f && faceDistance >= 20000.f)
	{
		//UE_LOG(LogTemp, Log, TEXT("LOD 2"));
		resolution = 128;
		detailLevel = 2;
	}
	else if (faceDistance < 20000.f)
	{
		//UE_LOG(LogTemp, Log, TEXT("LOD 3"));
		resolution = 256;
		detailLevel = 3;
	}

	BuildMesh(resolution);
}
*/

void UTerrain::ResetData()
{
	terrainData.vertices.Reset();
	terrainData.normals.Reset();
	terrainData.triangles.Reset();
	terrainData.uvs.Reset();
	terrainData.vertexColors.Reset();
	terrainData.tangents.Reset();

	waterData.vertices.Reset();
	waterData.normals.Reset();
	waterData.triangles.Reset();
	waterData.uvs.Reset();
	waterData.vertexColors.Reset();
	waterData.tangents.Reset();
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
	FVector dir = vertexPos;// -mesh->GetRelativeLocation();
	dir.Normalize();
	return dir.GetSafeNormal();
}

void UTerrain::SetColors()
{
	dynamicTerrainMat->SetVectorParameterValue("Terrain Data", FLinearColor(planetSettings->minHeight, planetSettings->maxHeight, planetSettings->radius, 0));
	dynamicTerrainMat->SetTextureParameterValue("Biome Sample", planetSettings->terrainGradient);
	mesh->SetMaterial(0, dynamicTerrainMat);
}


//Chunk Class

Chunk::Chunk(Chunk* parent, FVector location, float scale, int detailLevel, FVector localUp,
	FVector axisA, FVector axisB, FPlanetSettings* planetSettings, AActor* parentPlanet, FString id)
{
	children.SetNum(0);
	this->parent = parent;
	this->location = location;
	this->scale = scale;
	this->detailLevel = detailLevel;
	this->localUp = localUp;
	this->axisA = axisA;
	this->axisB = axisB;
	this->planetSettings = planetSettings;
	this->id = id;
	this->parentPlanet = parentPlanet;
}

bool Chunk::GenerateChildren(FVector cameraLocation)
{
	if (detailLevel < 0)
	{
		detailLevel = 0;
		return true;
	}


	bool modified = false;

	if (detailLevel <= maxLOD)
	{
		//UE_LOG(LogTemp, Log, TEXT("ID: %s"), *id);
		//UE_LOG(LogTemp, Log, TEXT("LOD: %i"), detailLevel);
		FVector surfaceLocation = (location * planetSettings->radius) + parentPlanet->GetActorLocation();
		//UE_LOG(LogTemp, Log, TEXT("Location: %f,%f,%f"), surfaceLocation.X, surfaceLocation.Y, surfaceLocation.Z);
		float distance = FVector::Distance(surfaceLocation, cameraLocation);
		//UE_LOG(LogTemp, Log, TEXT("Distance: %f\n"), distance);
		if (distance <= detailDistances[detailLevel] * planetSettings->radius)
		{
			if (children.Num() < 4)
			{
				//UE_LOG(LogTemp, Log, TEXT("GENERATING NEW CHILDREN"));
				children.SetNum(4);
				TArray<Chunk*> newChunks;
				children[0] = new Chunk(this,
					location + axisA * scale / 2.f + axisB * scale / 2.f, scale / 2.f, 
					detailLevel + 1, localUp, axisA, axisB, planetSettings, parentPlanet, id +"0");

				children[1] = new Chunk(this,
					location + axisA * scale / 2.f - axisB * scale / 2.f, scale / 2.f, 
					detailLevel + 1, localUp, axisA, axisB, planetSettings, parentPlanet, id + "1");

				children[2] = new Chunk(this,
					location - axisA * scale / 2.f + axisB * scale / 2.f, scale / 2.f, 
					detailLevel + 1, localUp, axisA, axisB, planetSettings, parentPlanet, id + "2");

				children[3] = new Chunk(this,
					location - axisA * scale / 2.f - axisB * scale / 2.f, scale / 2.f, 
					detailLevel + 1, localUp, axisA, axisB, planetSettings, parentPlanet, + "3");

				modified = true;
			}

			//UE_LOG(LogTemp, Log, TEXT("UPDATING CHILDREN"));
			for (auto& child : children)
			{
				if (child->GenerateChildren(cameraLocation))
					modified = true;
			}
		}
		else if (children.Num() >= 4)
		{
			//UE_LOG(LogTemp, Log, TEXT("REMOVING OLD CHILDREN"));
			children.Empty();
			modified = true;
		}
		//UE_LOG(LogTemp, Log, TEXT("END OF BRANCH"));
	}
	return modified;
}

TArray<Chunk*> Chunk::GetVisibleChildren()
{
	TArray<Chunk*> chunksToRender;

	if (children.Num() > 0)
	{
		for (auto &child : children)
		{
			chunksToRender.Append(child->GetVisibleChildren());
		}
	}
	else
	{
		chunksToRender.Add(this);
	}

	return chunksToRender;
}

FTriangleData Chunk::CalcuateTriangles(int triangleOffset)
{
	const int resolution = planetSettings->chunkResolution;
	FTriangleData data;
	int tri = 0;
	FProcMeshTangent tangent = FProcMeshTangent(0.f, 1.f, 0.f);
	for (int y = 0; y < resolution; y++)
	{
		for (int x = 0; x < resolution; x++)
		{
			int i = triangleOffset + x + y * resolution;
			FVector2D percent = (FVector2D(x, y) / (resolution - 1));

			FVector pointOnUnitCube = location +
				((percent.Y - 0.5f) * 2 * axisA + (percent.X - 0.5f) * 2 * axisB) *scale;
			FVector pointOnUnitSphere = UTerrain::CubeToSphere(pointOnUnitCube);

			float elevation = 0;
			elevation = SurfaceGenerator::ApplyNoise(pointOnUnitSphere * planetSettings->radius, planetSettings);
			FVector vertex = pointOnUnitSphere * planetSettings->radius * (1 + elevation);

			data.vertices.Add(vertex);
			data.normals.Add(UTerrain::CalculateNormal(vertex));
			data.tangents.Add(tangent);
			data.vertexColors.Add(FColor(elevation*255, elevation * 255, elevation * 255));
			data.uvs.Add(percent);

			if (x != resolution - 1 && y != resolution - 1)
			{
				data.triangles.Add(i);
				data.triangles.Add(i + resolution + 1);
				data.triangles.Add(i + resolution);

				data.triangles.Add(i);
				data.triangles.Add(i + 1);
				data.triangles.Add(i + resolution + 1);
				tri += 6;
			}
		}
	}

	return data;
}

void Chunk::CalculateTerrainAndWaterTris(FTriangleData& terrainData, FTriangleData& waterData)
{
	const int resolution = planetSettings->chunkResolution;
	int tri = 0;
	int triOffset = terrainData.vertices.Num();
	FProcMeshTangent tangent = FProcMeshTangent(0.f, 1.f, 0.f);
	for (int y = 0; y < resolution; y++)
	{
		for (int x = 0; x < resolution; x++)
		{
			FVector2D percent = (FVector2D(x, y) / (resolution - 1));

			FVector pointOnUnitCube = location +
				((percent.Y - 0.5f) * 2 * axisA + (percent.X - 0.5f) * 2 * axisB) * scale;
			FVector pointOnUnitSphere = UTerrain::CubeToSphere(pointOnUnitCube);

			float unitElevation = 0;
			unitElevation = SurfaceGenerator::ApplyNoise(pointOnUnitSphere * planetSettings->radius, planetSettings);
			float elevation = planetSettings->radius * (1 + unitElevation);
			SaveMinMaxValues(elevation);

			FVector vertex = pointOnUnitSphere * elevation;
			FVector waterVertex = pointOnUnitSphere * planetSettings->radius * planetSettings->waterHeight;//(1 + surfaceSettings->noiseSettings[0].simpleNoiseSettings.strength * 0.3f);

			CreateTriangle(terrainData, vertex, percent, resolution, triOffset, x, y, unitElevation);
			CreateTriangle(waterData, waterVertex, percent, resolution, triOffset, x, y);
			
			if (x != resolution - 1 && y != resolution - 1)
				tri += 6;
		}
	}
}

void Chunk::CreateTriangle(FTriangleData& triangleData, FVector vertexPos, FVector2D percent, int resolution,
	int triOffset, int x, int y, float elevation)
{
	int i = triOffset + x + y * resolution;

	triangleData.vertices.Add(vertexPos);
	triangleData.normals.Add(UTerrain::CalculateNormal(vertexPos));
	triangleData.tangents.Add(FProcMeshTangent(0.f, 1.f, 0.f));
	triangleData.vertexColors.Add(FColor(elevation * 255, elevation * 255, elevation * 255));
	triangleData.uvs.Add(percent);

	if (x != resolution - 1 && y != resolution - 1)
	{
		triangleData.triangles.Add(i);
		triangleData.triangles.Add(i + resolution + 1);
		triangleData.triangles.Add(i + resolution);

		triangleData.triangles.Add(i);
		triangleData.triangles.Add(i + 1);
		triangleData.triangles.Add(i + resolution + 1);
	}
}

void Chunk::SaveMinMaxValues(float elevation)
{
	if (planetSettings->minHeight > elevation || planetSettings->minHeight == -1)
		planetSettings->minHeight = elevation;

	if (planetSettings->maxHeight < elevation || planetSettings->maxHeight == -1)
		planetSettings->maxHeight = elevation;
}
