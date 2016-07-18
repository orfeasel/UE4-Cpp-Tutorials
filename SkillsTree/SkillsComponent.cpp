// Fill out your copyright notice in the Description page of Project Settings.

#include "TLSkillsTree.h"
#include "SkillsComponent.h"


// Sets default values for this component's properties
USkillsComponent::USkillsComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;

}

// Called when the game starts
void USkillsComponent::BeginPlay()
{
	Super::BeginPlay();

	//Reseting the level of each skill
	for (auto Skill : SkillsArray) Skill->GetDefaultObject<ASkill>()->ResetLevel();

	AvailableSkillPoints = InitialAvailableSkillsPoints;
}


UTexture* USkillsComponent::GetSkillTexture(int32 SkillNum)
{
	if (SkillsArray.IsValidIndex(SkillNum))
	{
		return SkillsArray[SkillNum]->GetDefaultObject<ASkill>()->GetSkillTexture();
	}
	return nullptr;
}

int32 USkillsComponent::GetSkillLevel(int32 SkillNum)
{
	if (SkillsArray.IsValidIndex(SkillNum))
	{
		return SkillsArray[SkillNum]->GetDefaultObject<ASkill>()->GetLevel();
	}
	return 0;
}

ASkill* USkillsComponent::GetSkillByType(ESkillType SkillType)
{
	for (auto It : SkillsArray)
	{
		ASkill* Skill = It->GetDefaultObject<ASkill>();
		if (Skill->GetSkillType() == SkillType) return Skill;
	}
	return nullptr;
}

int32 USkillsComponent::AdvanceSkillLevel(ASkill* SkillToLevelUp)
{
	if (SkillToLevelUp && AvailableSkillPoints > 0 && !SkillToLevelUp->IsMaxLevel())
	{
		AvailableSkillPoints--;
		SkillToLevelUp->AdvanceLevel();
		return SkillToLevelUp->GetLevel();
	}
	else if (SkillToLevelUp) return SkillToLevelUp->GetLevel();
	else return 0;
}

void USkillsComponent::ResetSkillPoints()
{
	AvailableSkillPoints = InitialAvailableSkillsPoints;
	for (auto It : SkillsArray)
	{
		It->GetDefaultObject<ASkill>()->ResetLevel();
	}
}

