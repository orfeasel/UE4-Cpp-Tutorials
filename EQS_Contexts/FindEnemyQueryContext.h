// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EnvironmentQuery/EnvQueryContext.h"
#include "FindEnemyQueryContext.generated.h"

/**
 * 
 */
UCLASS()
class EQSTUT_API UFindEnemyQueryContext : public UEnvQueryContext
{
	GENERATED_BODY()
	
	virtual void ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const override;
	
	
};
