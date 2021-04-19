// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBPFunctionLibrary.h"
#include "GameFramework/Actor.h"
#include "Engine/Engine.h"
#include "Engine/LatentActionManager.h"
#include "PingPongAction.h"


void UMyBPFunctionLibrary::PingPong(AActor* InActor, FVector LocA, FVector LocB, TEnumAsByte<EPingPongStatus::Type> PingPongStatus, FLatentActionInfo LatentInfo)
{
	if (UWorld* World = GEngine->GetWorldFromContextObjectChecked(InActor))
	{
		FLatentActionManager& LatentActionManager = World->GetLatentActionManager();

		FPingPongAction* PingPongAction = LatentActionManager.FindExistingAction<FPingPongAction>(LatentInfo.CallbackTarget,LatentInfo.UUID);
		
		//If not currently running
		if (!PingPongAction)
		{
			if (PingPongStatus == EPingPongStatus::Start)
			{
				PingPongAction = new FPingPongAction(InActor,LocA,LocB, LatentInfo);

				LatentActionManager.AddNewAction(LatentInfo.CallbackTarget, LatentInfo.UUID, PingPongAction);
			}
		}
		else
		{
			if (PingPongStatus == EPingPongStatus::Stop)
			{
				PingPongAction->bComplete=true;
			}
		}
	}
}
