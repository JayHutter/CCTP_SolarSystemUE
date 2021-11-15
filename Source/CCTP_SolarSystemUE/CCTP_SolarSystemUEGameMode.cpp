// Copyright Epic Games, Inc. All Rights Reserved.

#include "CCTP_SolarSystemUEGameMode.h"
#include "CCTP_SolarSystemUECharacter.h"
#include "UObject/ConstructorHelpers.h"

ACCTP_SolarSystemUEGameMode::ACCTP_SolarSystemUEGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
