// Fill out your copyright notice in the Description page of Project Settings.


#include "Tests/ComplexTest.h"
#include "Tests/AutomationEditorCommon.h" //for EngineAutomationTestUtilities
#include "Tests/AutomationCommon.h" //for FWaitLatentCommand or other latent commands
#include "FileHelpers.h"

#if WITH_DEV_AUTOMATION_TESTS

//Defining a complex test class named FComplexAutomationTest
IMPLEMENT_COMPLEX_AUTOMATION_TEST(FComplexAutomationTest,"TestCategory.LevelRelated.A Complex Test",EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::EngineFilter)

//Once we define a complex test, we need to override the GetTests function in order to build various parameters for our Tests. 
//The OutTestCommands will be contained in the Parameters parameter of the RunTest function
//The OutBeautifiedName will be used from the editor in the Automation tab as a name for each test
void FComplexAutomationTest::GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const
{
	TArray<FString> FileList;

#if WITH_EDITOR
	FEditorFileUtils::FindAllPackageFiles(FileList);
#else
	// Look directly on disk. Very slow!
	FPackageName::FindPackagesInDirectory(FileList, *FPaths::ProjectContentDir());
#endif

	// Iterate over all files, adding the ones with the map extension
	for (int32 FileIndex = 0; FileIndex < FileList.Num(); FileIndex++)
	{
		const FString& Filename = FileList[FileIndex];

		//Get all files that have a .umap extension
		if (FPaths::GetExtension(Filename, true) == FPackageName::GetMapPackageExtension())
		{
			//OutBeautifiedName contain names such as: ThirdPersonDefaultMap
			//OutTestCommands contain names such as: <CompletePathToYourProject>/ThirdPersonDefaultMap.umap
			OutBeautifiedNames.Add(FPaths::GetBaseFilename(Filename));
			OutTestCommands.Add(Filename);	
		}
	}
}

bool FComplexAutomationTest::RunTest(const FString& Parameters)
{
	//Retrieve the map name
	FString MapName = Parameters;

	//Loads given map name
	FAutomationEditorCommonUtils::LoadMap(MapName);

	//Add a new command to our queue
	ADD_LATENT_AUTOMATION_COMMAND(FRandomDelayCommand(1.f,3.f));

	return true;
}

#endif //WITH_DEV_AUTOMATION_TESTS

bool FRandomDelayCommand::Update()
{
	if (!bTestStarted)
	{
		Delay = FMath::RandRange(MinDelay, MaxDelay);
		GLog->Log("Started update!");
		GLog->Log("Delay value:"+FString::SanitizeFloat(Delay));
		StartedTime=FDateTime::Now();
		bTestStarted = true;
	}
	else if((FDateTime::Now().GetSecond() - StartedTime.GetSecond())>=Delay)
	{
		GLog->Log("finished waiting latent task!");
		GLog->Log("Elapsed time:"+FString::FromInt(FDateTime::Now().GetSecond() - StartedTime.GetSecond()));
		return true;
	}
	return false;
}
