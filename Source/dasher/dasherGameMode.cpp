// Copyright Epic Games, Inc. All Rights Reserved.

#include "DasherGameMode.h"
#include "DasherCharacter.h"
#include "UObject/ConstructorHelpers.h"

ADasherGameMode::ADasherGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
