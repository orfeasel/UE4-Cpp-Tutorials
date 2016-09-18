// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "SELethalShot.h"
#include "SELethalShotProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"

void ASELethalShotProjectile::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!bActivatedTransition && EnemyToKill && (EnemyToKill->GetActorLocation() - GetActorLocation()).Size() <= DeathCameraTransitionDistance)
	{
		//We're near the enemy, enable the camera transition
		EnemyToKill->EnableCameraTransition();
		bActivatedTransition = true;
	}
}

void ASELethalShotProjectile::ApplyVelocityMultiplier()
{
	ProjectileMovement->Velocity *= VelocityMultiplier;

	//We need to explicitely tell the projectile movement to update it's velocity after we adjust it's value
	ProjectileMovement->UpdateComponentVelocity();
}

ASELethalShotProjectile::ASELethalShotProjectile() 
{
	PrimaryActorTick.bCanEverTick = true;

	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &ASELethalShotProjectile::OnHit);		// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 5000.f;

	//Don't clamp the max speed of the projectile
	ProjectileMovement->MaxSpeed = 0.f;

	ProjectileMovement->bRotationFollowsVelocity = true;

	//Disable the bounce effect on projectiles
	ProjectileMovement->bShouldBounce = false;

	//We want this projectile to exist until it hits something so don't set an initial life span
	InitialLifeSpan = 0.f;

	//Create the spring arm component
	ProjectileSpringArmComp = CreateDefaultSubobject<USpringArmComponent>(FName("ProjectileSpringArmComp"));

	//Attach it to our root component
	ProjectileSpringArmComp->SetupAttachment(GetRootComponent());

	//Create the camera component
	ProjectileCameraComp = CreateDefaultSubobject<UCameraComponent>(FName("ProjectileCameraComp"));

	//Attach it to our spring arm component
	ProjectileCameraComp->SetupAttachment(ProjectileSpringArmComp);

}

void ASELethalShotProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if ((OtherActor != NULL) && (OtherActor != this))
	{
		if (OtherActor->IsA<ADummyEnemyCharacter>())
		{
			Cast<ADummyEnemyCharacter>(OtherActor)->Die();
		}
		Destroy();
	}
}