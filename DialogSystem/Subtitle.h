// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "Engine/DataTable.h"
#include "Subtitle.generated.h"

///**
// * 
// */
//UCLASS()
//class DIALOGSYSTEM_API USubtitle : public UObject
//{
//	GENERATED_BODY()
//	
//	
//	
//	
//};

USTRUCT(BlueprintType)
struct FSubtitle : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	/*The subtitle that will be displayed for a specific period of time in our UI*/
	UPROPERTY(EditAnywhere)
	FString Subtitle;

	/*The relative time in seconds, that the subtitle will appear*/
	UPROPERTY(EditAnywhere)
	float AssociatedTime;
};