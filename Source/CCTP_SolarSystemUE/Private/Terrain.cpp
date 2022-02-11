// Fill out your copyright notice in the Description page of Project Settings.


#include "Terrain.h"

// Sets default values for this component's properties
UTerrain::UTerrain()
{
	mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Procedural Mesh"));
	basicTangent = FProcMeshTangent(0.f, 1.f, 0.f);
	PrimaryComponentTick.bCanEverTick = true;
}

void UTerrain::Init(USurfaceSettings* _settings, FVector _localUp)//, int32 _resolution)
{
	//resolution = _resolution;
	localUp = _localUp;

	if (_settings)
		surfaceSettings = _settings;

	axisA = FVector(localUp.Z, localUp.X, localUp.Y);
	axisB = FVector::CrossProduct(localUp, axisA);

	faceLocation = localUp * surfaceSettings->radius;
	planetSeed = GetOwner()->GetActorLocation();
	mesh->SetRelativeLocation(FVector::ZeroVector);
}

// Called when the game starts
//void UTerrain::BeginPlay()
//{
//	Super::BeginPlay();	
//}
//
//void UTerrain::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//}

void UTerrain::BuildMesh(int resolution)
{
	mesh->SetRelativeLocation(FVector::ZeroVector);
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

void UTerrain::ConstructQuadTree()
{
	ResetData();

	FVector camLocation = GetWorld()->GetFirstPlayerController()->PlayerCameraManager->GetCameraLocation();
	//UE_LOG(LogTemp, Log, TEXT("Cam: %f,%f,%f"), camLocation.X, camLocation.Y, camLocation.Z);
	Chunk* parentChunk = new Chunk(nullptr, localUp, GetOwner()->GetActorLocation(),1.f,
		0, localUp, axisA, axisB, surfaceSettings);
	parentChunk->GenerateChildren(camLocation);
	
	int i = 0;
	//UE_LOG(LogTemp, Log, TEXT("Gen Mesh"));
	int triOffset = 0;
	for (auto &child : parentChunk->GetVisibleChildren())
	{
		//UE_LOG(LogTemp, Log, TEXT("i: %i"), i);
		FVector loc = child->location;
		//UE_LOG(LogTemp, Log, TEXT("Up: %f,%f,%f"), loc.X, loc.Y, loc.Z);
		FTriangleData triangleData = child->CalcuateTriangles(triOffset);
		vertices.Append(triangleData.vertices);
		triangles.Append(triangleData.triangles);
		normals.Append(triangleData.normals);
		uvs.Append(triangleData.uvs);
		vertexColors.Append(triangleData.vertexColors);
		tangents.Append(triangleData.tangents);
		triOffset += triangleData.vertices.Num();
		
		i++;
	}
	
	mesh->ClearAllMeshSections();
	mesh->CreateMeshSection(i, vertices, triangles, normals, uvs, vertexColors, tangents, true);
	mesh->SetRelativeLocation(FVector::ZeroVector, false, nullptr, ETeleportType::TeleportPhysics);
}

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

void UTerrain::ResetData()
{
	vertices.Reset();
	normals.Reset();
	triangles.Reset();
	uvs.Reset();
	vertexColors.Reset();
	tangents.Reset();
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



//Chunk Class
Chunk::Chunk(Chunk* parent, FVector location, FVector planetLocation, float scale, int detailLevel,
	FVector localUp, FVector axisA, FVector axisB, USurfaceSettings* surfaceSettings)
{
	children.SetNum(0);
	this->parent = parent;
	this->location = location;
	this->planetLocation = planetLocation;
	this->scale = scale;
	this->detailLevel = detailLevel;
	this->localUp = localUp;
	this->axisA = axisA;
	this->axisB = axisB;
	this->surfaceSettings = surfaceSettings;
}

void Chunk::GenerateChildren(FVector cameraLocation)
{
	if (detailLevel <= maxLOD)// && detailLevel >= 0)
	{
		UE_LOG(LogTemp, Log, TEXT("LOD: %i"), detailLevel);
		UE_LOG(LogTemp, Log, TEXT("Cam Location: %f,%f,%f"), cameraLocation.Z, cameraLocation.Y, cameraLocation.Z);
		FVector surfaceLocation = (location * surfaceSettings->radius) + planetLocation;
		UE_LOG(LogTemp, Log, TEXT("Location: %f,%f,%f"), surfaceLocation.X, surfaceLocation.Y, surfaceLocation.Z);
		float distance = FVector::Distance(surfaceLocation, cameraLocation);
		UE_LOG(LogTemp, Log, TEXT("Distance: %f\n"), distance);
		if (distance <= detailDistances[detailLevel] * surfaceSettings->radius)
		{
			children.SetNum(4);
			TArray<Chunk*> newChunks;
			children[0] = new Chunk(this,
				location + axisA * scale / 2.f + axisB * scale / 2.f, planetLocation,
				scale/2.f, detailLevel + 1, localUp, axisA, axisB, surfaceSettings);

			children[1] = new Chunk(this,
				location + axisA * scale / 2.f - axisB * scale / 2.f, planetLocation,
				scale / 2.f, detailLevel + 1, localUp, axisA, axisB, surfaceSettings);
			
			children[2] = new Chunk( this,
				location - axisA * scale / 2.f + axisB * scale / 2.f, planetLocation,
				scale / 2.f, detailLevel + 1, localUp, axisA, axisB, surfaceSettings);
			
			children[3] = new Chunk(this,
				location - axisA * scale / 2.f - axisB * scale / 2.f, planetLocation,
				scale / 2.f, detailLevel + 1, localUp, axisA, axisB, surfaceSettings);

			for (auto &child : children)
			{
				child->GenerateChildren(cameraLocation);
			}
		}
	}
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
	const int resolution = 8;
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
			elevation = SurfaceGenerator::ApplyNoise(pointOnUnitSphere, surfaceSettings);
			FVector vertex = pointOnUnitSphere * surfaceSettings->radius * (1 + elevation);

			data.vertices.Add(vertex);
			data.normals.Add(UTerrain::CalculateNormal(vertex));
			data.tangents.Add(tangent);
			data.vertexColors.Add(FColor::Green);
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
