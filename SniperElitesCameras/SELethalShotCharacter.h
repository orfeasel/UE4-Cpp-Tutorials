// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "SELethalShotProjectile.h"
#include "DummyEnemyCharacter.h"
#include "SELethalShotCharacter.generated.h"

class UInputComponent;

UCLASS(config=Game)
class ASELethalShotCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	class USkeletalMeshComponent* Mesh1P;

	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* FP_Gun;

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USceneComponent* FP_MuzzleLocation;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;

private:

	/*De-activates the default camera and activates the third person camera*/
	void ActivateThirdPersonCamera();

private:

	/*Returns true if the projectile hits an enemy - false otherwise*/
	bool HitsAnEnemy(ASELethalShotProjectile* Projectile, ADummyEnemyCharacter*& HitEnemy);

	/*Performs a raycast and returns the hit actor - if any*/
	AActor* GetHitActor(ASELethalShotProjectile* Projectile);

protected:

	/*The raycast length*/
	UPROPERTY(EditAnywhere)
	float RaycastLength = 2000.f;

private:

	/*De-activates the third person camera and activates the camera on the given projectile*/
	UFUNCTION()
	void ActivateProjectileCamera(ASELethalShotProjectile* Projectile);

protected:

	/*The time in seconds that we will transition from the third person camera to the projectile's camera*/
	UPROPERTY(EditAnywhere)
	float ThirdPersonCameraToProjectileCameraBlendTime = 0.05f;

	/*The delay in seconds that we will activate the projectile's camera*/
	UPROPERTY(EditAnywhere)
	float ThirdPersonToProjectileTransitionDelay = 0.005f;

protected:

	/*The time dilation multiplier*/
	UPROPERTY(EditAnywhere)
	float TimeDilationMultiplier = 0.05f;


	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* ThirdPersonSpringArmComp;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* ThirdPersonCameraComp;

public:

	/*Enables the FirstPersonCamera again*/
	void ResetActiveCamera();


public:
	ASELethalShotCharacter();

	virtual void BeginPlay();


	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector GunOffset;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class ASELethalShotProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	class USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* FireAnimation;

protected:
	
	/** Fires a projectile. */
	void OnFire();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

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

	struct TouchData
	{
		TouchData() { bIsPressed = false;Location=FVector::ZeroVector;}
		bool bIsPressed;
		ETouchIndex::Type FingerIndex;
		FVector Location;
		bool bMoved;
	};
	void BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location);
	TouchData	TouchItem;
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

	/* 
	 * Configures input for touchscreen devices if there is a valid touch interface for doing so 
	 *
	 * @param	InputComponent	The input component pointer to bind controls to
	 * @returns true if touch controls were enabled.
	 */
	bool EnableTouchscreenMovement(UInputComponent* InputComponent);

public:
	/** Returns Mesh1P subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

};

