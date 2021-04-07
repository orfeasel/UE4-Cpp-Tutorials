// Copyright Epic Games, Inc. All Rights Reserved.

#include "AutomationPostGameMode.h"
#include "AutomationPostCharacter.h"
#include "UObject/ConstructorHelpers.h"

AAutomationPostGameMode::AAutomationPostGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
