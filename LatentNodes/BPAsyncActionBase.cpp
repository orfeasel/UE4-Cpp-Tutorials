// Fill out your copyright notice in the Description page of Project Settings.


#include "BPAsyncActionBase.h"

bool UBPAsyncActionBase::bActive=false; //Init bactive for all instances

int32 UBPAsyncActionBase::FibonacciNum(int32 N)
{
	if (N == 0)
	{
		return 0;
	}
	else if (N == 1)
	{
		return 1;
	}
	else
	{
		return FibonacciNum(N-1) + FibonacciNum(N-2);
	}
}

void UBPAsyncActionBase::InternalTick()
{
	FiboNum++;
	LastResult = FibonacciNum(FiboNum);
	Tick.Broadcast(LastResult);
	
	if (FiboNum >= NumsToCalculate)
	{
		InternalCompleted();
	}
}

void UBPAsyncActionBase::InternalCompleted()
{
	if (WorldContext)
	{
		WorldContext->GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
		TimerHandle.Invalidate();
		Completed.Broadcast(LastResult);
		UBPAsyncActionBase::bActive=false;
	}
}

UBPAsyncActionBase* UBPAsyncActionBase::BPAsyncNode(const UObject* WorldContextObj, int32 Num)
{
	UBPAsyncActionBase* Node = NewObject<UBPAsyncActionBase>();
	if (Node)
	{
		Node->WorldContext = WorldContextObj;
		Node->NumsToCalculate=Num;
	}
	return Node;
}

void UBPAsyncActionBase::Activate()
{
	if (UBPAsyncActionBase::bActive)
	{
		FFrame::KismetExecutionMessage(TEXT("Async action is already running"), ELogVerbosity::Warning);
		return;
	}

	FFrame::KismetExecutionMessage(TEXT("Started Activate!"), ELogVerbosity::Log);

	if (WorldContext)
	{
		UBPAsyncActionBase::bActive=true;
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindUObject(this,&UBPAsyncActionBase::InternalTick);
		WorldContext->GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, 0.1f, true);
	}
	else
	{
		FFrame::KismetExecutionMessage(TEXT("Invalid world context obj"), ELogVerbosity::Error);
	}

}
