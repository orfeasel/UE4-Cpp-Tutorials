// Fill out your copyright notice in the Description page of Project Settings.


#include "Tests/LevelTests.h"
#include "Misc/AutomationTest.h"
#include "Tests/AutomationEditorCommon.h"
#include "Editor.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "AutomationPost/AutomationPostCharacter.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FValidPlayerLevelTest, "TestCategory.LevelRelated.Check for Valid Players", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FValidPlayerLevelTest::RunTest(const FString& Parameters)
{
	//Checking if there is at least one player placed in the world
	
	UWorld* World = GEditor->GetWorld();
	{
		if (World)
		{
			ACharacter* PlayerChar = UGameplayStatics::GetPlayerCharacter(World,0);

			//For more tests check AutomationTest.h (lines 1347 - 1639)
			TestNotNull<ACharacter>(TEXT("Testing if there is a player actor in level"),PlayerChar);
		}
	}

	return true;
}

#endif //WITH_DEV_AUTOMATION_TESTS