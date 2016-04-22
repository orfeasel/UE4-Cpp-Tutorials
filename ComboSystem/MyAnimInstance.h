// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Animation/AnimInstance.h"
#include "MyAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class MELEECOMBATSYSTEM_API UMyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:

	/*Holds a reference to the character's movement speed*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float MovementSpeed;

	/*The attack montage*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAnimMontage* MeleeAttackMontage;

	/*Updates the Movement Speed variable*/
	UFUNCTION(BlueprintCallable, Category = "AnimationUpdates")
	void UpdateMovementSpeed();

	/*True if the character can perform a second attack*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bAcceptsSecondAttackInput;

	/*Ture if the character can perform a third attack*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bAcceptsThirdAttackInput;

public:
	/*Determines which attack animation will be played*/
	void Attack();
};
