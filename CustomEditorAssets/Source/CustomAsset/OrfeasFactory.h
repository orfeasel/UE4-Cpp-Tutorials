// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "OrfeasFactory.generated.h"

/**
 * 
 */
UCLASS()
class CUSTOMASSET_API UOrfeasFactory : public UFactory
{
	GENERATED_BODY()

public:

	UOrfeasFactory();

	/* New assets that don't override this function are automatically placed into the "Miscellaneous" category in the editor */
	virtual uint32 GetMenuCategories() const override;
	
	
	/* Creates the asset inside the UE4 Editor */
	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	
};
