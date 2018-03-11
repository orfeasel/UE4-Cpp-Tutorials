// Fill out your copyright notice in the Description page of Project Settings.

#include "JsonParser.h"
#include "JsonUtilities.h"

// Sets default values
AJsonParser::AJsonParser()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AJsonParser::BeginPlay()
{
	Super::BeginPlay();

	const FString JsonFilePath = FPaths::ProjectContentDir() + "/JsonFiles/randomgenerated.json";
	FString JsonString; //Json converted to FString
	
	FFileHelper::LoadFileToString(JsonString,*JsonFilePath);

	//Displaying the json in a string format inside the output log
	GLog->Log("Json String:");
	GLog->Log(JsonString);

	//Create a json object to store the information from the json string
	//The json reader is used to deserialize the json object later on
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonString);

	if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
	{
		//The person "object" that is retrieved from the given json file
		TSharedPtr<FJsonObject> PersonObject = JsonObject->GetObjectField("Person");

		//Getting various properties
		GLog->Log("Balance:" + PersonObject->GetStringField("balance"));
		GLog->Log("Age:" + FString::FromInt(PersonObject->GetIntegerField("age")));
		FString IsActiveStr = (PersonObject->GetBoolField("isActive")) ? "Active" : "Inactive";
		GLog->Log("IsActive:" + IsActiveStr);
		GLog->Log("Latitude:" + FString::SanitizeFloat(PersonObject->GetNumberField("latitude")));

		//Retrieving an array property and printing each field
		TArray<TSharedPtr<FJsonValue>> objArray=PersonObject->GetArrayField("family");
		GLog->Log("printing family names...");
		for(int32 index=0;index<objArray.Num();index++)
		{
			
			GLog->Log("name:"+objArray[index]->AsString());
		}
	}
	else
	{
		GLog->Log("couldn't deserialize");
	}

}

// Called every frame
void AJsonParser::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

