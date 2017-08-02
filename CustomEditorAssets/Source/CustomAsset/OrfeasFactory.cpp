// Fill out your copyright notice in the Description page of Project Settings.

#include "OrfeasFactory.h"
//The asset header file that we wish to create
#include "OrfeasCustomAsset.h"
//The asset type categories will let us access the various asset categories inside the Editor
#include "AssetTypeCategories.h"


UOrfeasFactory::UOrfeasFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;
	//Configure the class that this factory creates
	SupportedClass = UOrfeasCustomAsset::StaticClass();
}

uint32 UOrfeasFactory::GetMenuCategories() const
{
	//Let's place this asset in the Blueprints category in the Editor
	return EAssetTypeCategories::Blueprint;
}

UObject* UOrfeasFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	//Create the editor asset 
	UOrfeasCustomAsset* OrfeasEditorAsset = NewObject<UOrfeasCustomAsset>(InParent, InClass, InName, Flags);
	return OrfeasEditorAsset;
}
