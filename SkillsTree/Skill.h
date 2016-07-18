// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Skill.generated.h"


UENUM(BlueprintType)
enum class ESkillType : uint8
{
	WaterBlob,
	FireBall
};

UCLASS()
class TLSKILLSTREE_API ASkill : public AActor
{
	GENERATED_BODY()

private:
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
public:	
	// Sets default values for this actor's properties
	ASkill();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

	/*Increases the level by one - clamps on max level*/
	UFUNCTION(BlueprintCallable,Category=TLSkillsTree)
	void AdvanceLevel() { CurrentLevel = (CurrentLevel + 1 > MaxLevel) ? 1 : ++CurrentLevel; }

	/*Resets the level of the skill - 0 means that the player will not be able to fire*/
	UFUNCTION(BlueprintCallable,Category=TLSkillsTree)
	void ResetLevel() { CurrentLevel = 0; }

	/*Returns the level of the current skill*/
	UFUNCTION(BlueprintCallable,Category=TLSkillsTree)
	int32 GetLevel() { return CurrentLevel; }

	/*Returns the skill's texture*/
	UFUNCTION(BlueprintCallable,Category=TLSkillsTree)
	UTexture* GetSkillTexture() { return SkillTexture; }

	/*Returns the skill type*/
	ESkillType GetSkillType() { return SkillType; }

	/*Returns true if the level is maxed out*/
	bool IsMaxLevel() { return CurrentLevel == MaxLevel; }

private:
	int32 CurrentLevel = 1;

	int32 MaxLevel = 3;
	
protected:

	/*Sphere comp used for collision*/
	UPROPERTY(VisibleAnywhere)
	USphereComponent* SphereComp;

	/*This component is used to simulate the movement of our skill*/
	UPROPERTY(VisibleAnywhere)
	UProjectileMovementComponent* ProjectileMovementComp;
	
	/*The particle comp which emits the active particle system*/
	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* ParticleComp;

	/*The particle system for our projectile when traveling*/
	UPROPERTY(EditDefaultsOnly)
	UParticleSystem* ProjectileFX;

	/*The particle system for our collision*/
	UPROPERTY(EditDefaultsOnly)
	UParticleSystem* ProjectileCollisionFX;

	/*The skill texture*/
	UPROPERTY(EditDefaultsOnly)
	UTexture* SkillTexture;

	/*The time (after a collision has happened) that our skill will get destroyed*/
	UPROPERTY(EditAnywhere)
	float DestroyDelay = 1.5f;

	/*The skill type of the skill*/
	UPROPERTY(EditDefaultsOnly)
	ESkillType SkillType;
};
