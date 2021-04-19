// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/WeakObjectPtr.h"
#include "GameFramework/Actor.h"
#include "Engine/LatentActionManager.h"
#include "Engine/Public/LatentActions.h"


class LATENTNODES_API FPingPongAction : public FPendingLatentAction
{
public:

	
	/** Actor we're going to interpolate */
	AActor* ActorToPingPong;

	/** Starting location of interpolation */
	FVector InitialLocation;
	
	/** Target location of interpolation */
	FVector TargetLocation;

	/** True when we're stopping the movement */
	bool bComplete;

	/** Elapsed time of a single interpolation */
	float TimeElapsed;

	/** Total time for the interpolation from Initial to Target location*/
	float TotalTime;

	/** Function to execute on completion */
	FName ExecutionFunction;

	/** Link to fire on completion */

	int32 OutputLink;

	/**
	 * Object to call callback on upon completion
	 */
	FWeakObjectPtr CallbackTarget;

	FPingPongAction(AActor* InActor, FVector InitLoc, FVector TargetLoc, const FLatentActionInfo& LatentInfo) 
		: ActorToPingPong(InActor)
		, InitialLocation(InitLoc)
		, TargetLocation(TargetLoc)
		, bComplete(false)
		, TimeElapsed(0.f)
		, TotalTime(1.f)
		, ExecutionFunction(LatentInfo.ExecutionFunction)
		, OutputLink(LatentInfo.Linkage)
		, CallbackTarget(LatentInfo.CallbackTarget)
	{
	}

	virtual void UpdateOperation(FLatentResponse& Response) override
	{
		TimeElapsed+=Response.ElapsedTime();
		float Alpha = TimeElapsed / TotalTime;

		if (ActorToPingPong)
		{
			
			FVector NewLocation = FMath::Lerp(InitialLocation,TargetLocation,Alpha);

			ActorToPingPong->SetActorLocation(NewLocation);
			//If we have reached the target location swap initial and target
			//Hardcoded tolernace just for demonstration purposes.
			if (NewLocation.Equals(TargetLocation,15.f))
			{	
				FVector TempLocation = TargetLocation;
				TargetLocation = InitialLocation;
				InitialLocation = TempLocation;
				TimeElapsed=0.f; //Restart timer as well
			}

		}

		Response.FinishAndTriggerIf(bComplete || Alpha>=1.f,ExecutionFunction,OutputLink,CallbackTarget);
	}

#if WITH_EDITOR
	// Returns a human readable description of the latent operation's current state
	virtual FString GetDescription() const override
	{
		static const FNumberFormattingOptions DelayTimeFormatOptions = FNumberFormattingOptions()
			.SetMinimumFractionalDigits(3)
			.SetMaximumFractionalDigits(3);
		return FText::Format(NSLOCTEXT("FPingPongAction", "ActionTimeFmt", "Ping Pong ({0} seconds left)"), FText::AsNumber(TotalTime - TimeElapsed, &DelayTimeFormatOptions)).ToString();
	}
#endif
};
