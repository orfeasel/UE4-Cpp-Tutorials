// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "FlightStop.h"
#include "Components/TimelineComponent.h"
#include "WoWFlightCharacter.generated.h"

UCLASS(config=Game)
class AWoWFlightCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

private:

	/** The flight timeline */
	FTimeline FlightTimeline;

	/** The function that ticks the timeline */
	UFUNCTION()
	void TickTimeline(float Value);

	/** The active spline component, meaning the flight path that the character is currently following */
	USplineComponent* ActiveSplineComponent;

	/** The selected flight stop actor */
	AFlightStop* ActiveFlightStopActor;

	/** Box overlap function */
	UFUNCTION()
	void OnFlightBoxColliderOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** Executes when we're pressing the NextFlightPath key bind */
	void NextFlightPathSelected();

	/** Executes when we're pressing the PreviousFlightPath key bind */
	void PreviousFlightPathSelected();

	/** Updates the flight timeline with a new curve and starts the flight */
	void UpdateFlightTimeline(UCurveFloat* CurveFloatToBind);

	UFUNCTION()
	void ResetActiveFlightStopActor();

protected:

	/*The Box component that detects any nearby flight stops*/
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* FlightBoxCollider;

public:
	AWoWFlightCharacter();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

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

