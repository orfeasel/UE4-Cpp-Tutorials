// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "CoverActor.h"
#include "CoverSystemCharacter.generated.h"

UCLASS(config=Game)
class ACoverSystemCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

private:
	/** Enables or disables the crouch mode*/
	void ToggleCrouch();

	/**Enables or disables the cover mode*/
	void ToggleCover();

	/** True if the player can take cover */
	bool bCanTakeCover = false;

	/** True if the player is currently taking cover */
	bool bIsInCover = false;

	/** The movement direction while taking cover */
	FVector CoverDirectionMovement;

	/** Cover reference*/
	ACoverActor* Cover;

	/** Contains the input of the cover */
	float CoverValue;

public:

	/** Inform the player that he's able to take cover in the provided actor */
	void SetCanTakeCover(bool bCanTakeCover, ACoverActor* CoverActor);

	/** Returns the input while in cover */
	UFUNCTION(BlueprintCallable, Category = CoverSystem)
	float CoverValueInput() { return CoverValue; }

	/** Returns true if we're in cover*/
	UFUNCTION(BlueprintCallable, Category = CoverSystem)
	bool IsInCover() { return bIsInCover; }

	ACoverSystemCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;


protected:

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

