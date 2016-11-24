// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Bomb.generated.h"

UCLASS()
class NETWORKINGTUT_API ABomb : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABomb();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:

	/** The static mesh of the comp */
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* SM;

	/** The projectile movement comp */
	UPROPERTY(VisibleAnywhere)
	UProjectileMovementComponent* ProjectileMovementComp;

	/** Sphere comp used for collision. Movement component need a collision component as root to function properly */
	UPROPERTY(VisibleAnywhere)
	USphereComponent* SphereComp;

	/** The delay until explosion */
	UPROPERTY(EditAnywhere, Category = BombProps)
	float FuseTime = 2.5f;

	UPROPERTY(EditAnywhere, Category = BombProps)
	float ExplosionRadius = 200.f;

	UPROPERTY(EditAnywhere, Category = BombProps)
	float ExplosionDamage = 25.f;

	/** The particle system of the explosion */
	UPROPERTY(EditAnywhere)
	UParticleSystem* ExplosionFX;

private:

	/** Marks the properties we wish to replicate */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(ReplicatedUsing = OnRep_IsArmed)
	bool bIsArmed = false;

	/** Called when bIsArmed gets updated */
	UFUNCTION()
	void OnRep_IsArmed();

	/** Arms the bomb for explosion */
	void ArmBomb();

	/** Called when our bomb bounces */
	UFUNCTION()
	void OnProjectileBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity);

	/** Performs an explosion after a certain amount of time */
	void PerformDelayedExplosion(float ExplosionDelay);

	/** Performs an explosion when called */
	UFUNCTION()
	void Explode();

	//Simulate explosion functions
	
	/** 
	 * The multicast specifier, indicates that every client will call the SimulateExplosionFX_Implementation. 
	 * You don't have to generate an implementation for this function.
	 */
	UFUNCTION(Reliable, NetMulticast)
	void SimulateExplosionFX();

	/** The actual implementation of the SimulateExplosionFX */
	void SimulateExplosionFX_Implementation();
};
