// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Pickup.h"
#include "InventorySlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORYSYSTEM_API UInventorySlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	/*Holds a reference to the item texture*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UTexture2D* ItemTexture;

	/*Tells the player the equip the represented item from this widget*/
	UFUNCTION(BlueprintCallable, Category = UI)
	void SetEquippedItem();

public:
	/*Sets the item texture*/
	UFUNCTION(BlueprintCallable, Category = UI)
	void SetItemTexture(APickup* Item);
	
	
};
