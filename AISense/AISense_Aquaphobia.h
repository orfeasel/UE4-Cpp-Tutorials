// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Perception/AISense.h"
#include "AISense_Aquaphobia.generated.h"

/**
 * 
 */
class UAISenseConfig_Aquaphobia; //forward declaration to avoid circular dependencies

UCLASS(meta=(DisplayName="AI Aquaphobia config"))
class CUSTOMAISENSE_API UAISense_Aquaphobia : public UAISense
{
	GENERATED_BODY()

public:

	/*
	 * After inspecting the engine's code it seems like there's a pattern to use a struct
	 * to consume the properties from the config class so it's better to follow the engine's
	 * standards and workflows.
	 */
	struct FDigestedAquaProperties
	{
		float PhobiaRadius;
		bool bDisplayDebugSphere;

		FDigestedAquaProperties();
		FDigestedAquaProperties(const UAISenseConfig_Aquaphobia& SenseConfig);
	};

	/* Consumed properties from config */
	TArray<FDigestedAquaProperties> DigestedProperties;

	UAISense_Aquaphobia();
	
protected:

	/* Core logic for the sense */
	virtual float Update() override;
	
	/* A listener is someone who has a Perception component with various senses
	 * This function will be called when a new listener gained this sense 
	 */
	void OnNewListenerImpl(const FPerceptionListener& NewListener);

	/*
	 * Called whenever the listener is removed (eg destroyed or game has stopped)
	 */
	void OnListenerRemovedImpl(const FPerceptionListener& UpdatedListener);
};
