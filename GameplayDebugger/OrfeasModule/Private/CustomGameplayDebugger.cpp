// Fill out your copyright notice in the Description page of Project Settings.

#include "OrfeasModule.h"
#include "CustomGameplayDebugger.h"
#include "GDBlogPostCharacter.h"

#if WITH_GAMEPLAY_DEBUGGER

#include "DebugRenderSceneProxy.h"

FGameplayDebuggerCategory_Orfeas::FGameplayDebuggerCategory_Orfeas()
{
	bShowOnlyWithDebugActor = false;
}

TSharedRef<FGameplayDebuggerCategory> FGameplayDebuggerCategory_Orfeas::MakeInstance()
{
	return MakeShareable(new FGameplayDebuggerCategory_Orfeas());
}

void FGameplayDebuggerCategory_Orfeas::CollectData(APlayerController* OwnerPC, AActor* DebugActor)
{

	if (DebugActor)
	{
		AGDBlogPostCharacter* Char = Cast<AGDBlogPostCharacter>(DebugActor);

		if (Char)
		{
			//Store the data to our struct
			Data.HP = Char->Health;
			Data.Damage = Char->MaxDamage;
		}
	}
}

void FGameplayDebuggerCategory_Orfeas::DrawData(APlayerController* OwnerPC, FGameplayDebuggerCanvasContext& CanvasContext)
{
	//Test print with white text
	CanvasContext.Printf(TEXT("Test Print"));

	CanvasContext.Printf(FColor::Yellow, TEXT("Yet again another print!"));

	//Print the health data with green color
	CanvasContext.Printf(TEXT("{green}HP: %s"), *FString::SanitizeFloat(Data.HP));

	//Print the damage data with red color
	CanvasContext.Printf(TEXT("{red}Damage: %s"), *FString::SanitizeFloat(Data.Damage));
}

#endif