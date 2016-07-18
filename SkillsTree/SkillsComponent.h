// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "Skill.h"
#include "SkillsComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TLSKILLSTREE_API USkillsComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USkillsComponent();

	// Called when the game starts
	virtual void BeginPlay() override;

	/*An array which contains all the available skills*/
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<ASkill>> SkillsArray;

	/*Returns the texture of the given skill's index (searches SkillsArray)*/
	UFUNCTION(BlueprintCallable, Category = TLSkillsTree)
	UTexture* GetSkillTexture(int32 SkillNum);

	UFUNCTION(BlueprintCallable, Category = TLSkillsTree)
	int32 GetSkillLevel(int32 SkillNum);

	/*Returns the first matching skill from SkillsArray*/
	UFUNCTION(BlueprintCallable, Category = TLSkillsTree)
	ASkill* GetSkillByType(ESkillType SkillType);
	
private:
	/*The Available Skill Points which can be spent in total*/
	int32 AvailableSkillPoints;

public:

	/*Returns the new level of the skill*/
	UFUNCTION(BlueprintCallable, Category = TLSkillsTree)
	int32 AdvanceSkillLevel(ASkill* SkillToLevelUp);

	/*Resets the skill points and unlearns all the skills*/
	UFUNCTION(BlueprintCallable, Category = TLSkillsTree)
	void ResetSkillPoints();

protected:

	/*The amount of available skill points when starting the game*/
	UPROPERTY(EditDefaultsOnly)
	int32 InitialAvailableSkillsPoints;

};
