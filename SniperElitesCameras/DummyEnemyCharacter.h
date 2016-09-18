// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "DummyEnemyCharacter.generated.h"

UCLASS()
class SELETHALSHOT_API ADummyEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

protected:

	/*The blend time in seconds, from the projectile's camera to corresponding death camera*/
	UPROPERTY(EditAnywhere)
	float DeathCameraBlendTime = 0.05f;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* DeathSpringArmComp;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* DeathCameraComp;

public:

	/*Transitions the active camera to the corresponding death camera*/
	void EnableCameraTransition();

protected:

	/*The delay the camera will be reset right after the death of the enemy*/
	UPROPERTY(EditAnywhere)
	float CameraResetDelay = 1.f;

public:

	/*Kills the character and activates the FirstPerson camera again*/
	void Die();

public:
	// Sets default values for this character's properties
	ADummyEnemyCharacter();


	
	
};
