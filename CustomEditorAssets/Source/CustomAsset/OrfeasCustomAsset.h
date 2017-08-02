// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "OrfeasCustomAsset.generated.h"

/**
 * 
 */
UCLASS()
class CUSTOMASSET_API UOrfeasCustomAsset : public UObject
{
	GENERATED_BODY()
	
	
protected:

	//Just some properties to display on the Editor

	UPROPERTY(EditAnywhere)
	FString Description;

	UPROPERTY(EditAnywhere)
	int32 BonusCoins;
	
};
