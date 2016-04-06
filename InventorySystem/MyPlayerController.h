// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "InventoryWidget.h"
#include "MyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORYSYSTEM_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()
	
private:
	/*InventoryWidget reference*/
	UInventoryWidget* InventoryWidgetRef;

	/*True if the inventory is currently open*/
	bool bIsInventoryOpen;
	
protected:
	/*InventoryWidget Blueprint reference*/
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UInventoryWidget> InventoryWidgetBP;
	

public:
	virtual void Possess(APawn* InPawn) override;

	/*Opens or closes the inventory*/
	void HandleInventoryInput();

};
