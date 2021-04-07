// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h" //For IAutomationLatentCommand

/**
 * A command that gets completed after a random delay
 */
class FRandomDelayCommand : public IAutomationLatentCommand
{

private:

	/* Min delay in seconds */
	float MinDelay;

	/* Max delay in seconds */
	float MaxDelay;

	/* False by default, true when we have started the test */
	bool bTestStarted;

	/* The actual time in seconds we're going to delay */
	float Delay;

	/* Stores the time we have started our test */
	FDateTime StartedTime;

public:

	FRandomDelayCommand(float Min, float Max) : MinDelay(Min), MaxDelay(Max), bTestStarted(false), Delay(0.f) {}

	/**
	 * Will execute each frame and will stop once we return true
	 */
	virtual bool Update() override;
};