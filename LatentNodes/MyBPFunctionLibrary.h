// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MyBPFunctionLibrary.generated.h"

class AActor;

UENUM()
namespace EPingPongStatus
{
	enum Type
	{
		Start,
		Stop
	};
}

/**
 * 
 */
UCLASS()
class LATENTNODES_API UMyBPFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	/**
	 * Interpolates the given actor between the given locations
	 * @param InActor - the actor to interpolate
	 * @param LocA - the starting location of the interpolation
	 * @param LocB - the target location of the interpolation
	 * @param PingPongStatus - the status of the ping pong
	 * @param LatentInfo - the latent info to handle the progress in the background
	 */
	UFUNCTION(BlueprintCallable, meta = (Latent, LatentInfo = "LatentInfo", WorldContext = "WorldContextObject", ExpandEnumAsExecs = "PingPongStatus"), Category = "MyBPFunctions")
	static void PingPong(AActor* InActor, FVector LocA, FVector LocB, TEnumAsByte<EPingPongStatus::Type> PingPongStatus, FLatentActionInfo LatentInfo);

};
