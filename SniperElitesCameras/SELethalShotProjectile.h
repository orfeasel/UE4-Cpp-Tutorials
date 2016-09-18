// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Actor.h"
#include "DummyEnemyCharacter.h"
#include "SELethalShotProjectile.generated.h"

UCLASS(config=Game)
class ASELethalShotProjectile : public AActor
{
	GENERATED_BODY()

	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	class USphereComponent* CollisionComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* ProjectileMovement;

private:

	/*The enemy that this projectile is going to kill*/
	ADummyEnemyCharacter* EnemyToKill = nullptr;

	/*True when the transition from the projectile's camera to the enemy's camera has been activated*/
	bool bActivatedTransition = false;

protected:

	/*The distance threshold that the death camera transition will occur*/
	UPROPERTY(EditAnywhere)
	float DeathCameraTransitionDistance = 300.f;

public:

	virtual void Tick(float DeltaSeconds) override;

	/*Sets the enemy that is going to be killed*/
	FORCEINLINE void SetEnemyToKill(ADummyEnemyCharacter* Enemy) { EnemyToKill = Enemy; }

protected:

	/*The velocity multiplier that is applied when we slow the world's time*/
	UPROPERTY(EditAnywhere)
	float VelocityMultiplier = 5.f;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* ProjectileCameraComp;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* ProjectileSpringArmComp;

public:

	/** Applies the velocity multiplier to the existing velocity of the projectile */
	void ApplyVelocityMultiplier();

public:
	ASELethalShotProjectile();

	/** called when projectile hits something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/** Returns CollisionComp subobject **/
	FORCEINLINE class USphereComponent* GetCollisionComp() const { return CollisionComp; }
	/** Returns ProjectileMovement subobject **/
	FORCEINLINE class UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }
};

