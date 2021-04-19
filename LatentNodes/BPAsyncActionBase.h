// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "BPAsyncActionBase.generated.h"

/**
 * Signatures of execution pins in the editor
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBPNodeOutputPin,int32,FibonacciNumber);

/**
 * 
 */
UCLASS()
class LATENTNODES_API UBPAsyncActionBase : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
	
private:

	/**
	 * Keeps track of the last sequence number
	 * Used in order to stop the execution at some point
	 */
	int32 FiboNum;

	/**
	 * Contains the latest result from FibonacciNum
	 */
	int32 LastResult;

	/**
	 * Recursive way of computing a fibonacci number
	 * @param N the number of the sequence https://en.wikipedia.org/wiki/Fibonacci_number
	 */
	int32 FibonacciNum(int32 N);

	/**
	 * Internal tick. Will also broadcast the Tick Execution pin in the editor
	 */
	UFUNCTION()
	void InternalTick();

	/**
	 * Internal completed. Clears timers and flags and broadcasts Completed Execution pin in the editor
	 */
	UFUNCTION()
	void InternalCompleted();

	/**
	 * Static property to prevent restarting the async node multiple times before execution has finished
	 */
	static bool bActive;

	/**
	 * Numbers of fibonacci numbers to calculate
	 */
	int32 NumsToCalculate;

	/**
	 * World context object to grab a reference of the world 
	 */
	const UObject* WorldContext;

	/**
	 * Timer handle of internal tick
	 */
	FTimerHandle TimerHandle;

public:

	UPROPERTY(BlueprintAssignable)
	FBPNodeOutputPin Tick;

	UPROPERTY(BlueprintAssignable)
	FBPNodeOutputPin Completed;

	/**
	 * InternalUseOnly to hide sync version in BPs
	 */
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "AsyncNode")
	static UBPAsyncActionBase* BPAsyncNode(const UObject* WorldContextObj, int32 Num);

	//Overriding BP async action base
	virtual void Activate() override;
};
