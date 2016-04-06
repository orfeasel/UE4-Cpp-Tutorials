// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Pickup.h"
#include "InventoryWidget.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORYSYSTEM_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
	
	
protected:
	/*True if any animation is playing in order to have smooth UI changes (non overlapping animations)*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Animations)
	bool bIsAnimationPlaying = false;



public:
	/*Adds the widget into the viewport and plays its entry animation*/
	UFUNCTION(BlueprintImplementableEvent, Category = Animations)
	void Show();

	/*Removes the widget from the viewport and plays the outro animation*/
	UFUNCTION(BlueprintImplementableEvent, Category = Animations)
	void Hide();

	/*Stores a reference in order to bind information on inventory slots*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<APickup*> ItemsArray;

	/*True if any animation is playing -entry or outro-*/
	FORCEINLINE bool GetIsAnyAnimationActive() { return bIsAnimationPlaying; }

	/*Resets the equip status on all inventory slots*/
	UFUNCTION(BlueprintImplementableEvent, Category = UI)
	void ResetInventorySlotEquipStatus();
	
};
