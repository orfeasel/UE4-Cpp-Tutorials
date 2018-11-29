// Fill out your copyright notice in the Description page of Project Settings.

#include "AISenseConfig_Aquaphobia.h"
#include "AISense_Aquaphobia.h"




UAISenseConfig_Aquaphobia::UAISenseConfig_Aquaphobia(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	DebugColor = FColor::Blue;
	Implementation = GetSenseImplementation();		
}

TSubclassOf<UAISense> UAISenseConfig_Aquaphobia::GetSenseImplementation() const
{
	return *Implementation;
}
