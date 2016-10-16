// Fill out your copyright notice in the Description page of Project Settings.

#include "EqsTut.h"
#include "MyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "EqsTutCharacter.h"


void AMyAIController::OnPerceptionUpdated(TArray<AActor*> UpdatedActors)
{
	//If our character exists inside the UpdatedActors array, register him
	//to our blackboard component

	for (AActor* Actor : UpdatedActors)
	{
		if (Actor->IsA<AEqsTutCharacter>() && !GetSeeingPawn())
		{
			BlackboardComp->SetValueAsObject(BlackboardEnemyKey, Actor);
			return;
		}
	}

	//The character doesn't exist in our updated actors - so make sure
	//to delete any previous reference of him from the blackboard
	BlackboardComp->SetValueAsObject(BlackboardEnemyKey, nullptr);
}

AMyAIController::AMyAIController()
{
	//Components Init.
	BehaviorTreeComp = CreateDefaultSubobject<UBehaviorTreeComponent>(FName("BehaviorComp"));

	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(FName("BlackboardComp"));

	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(FName("PerceptionComp"));

	//Create a Sight Sense
	Sight = CreateDefaultSubobject<UAISenseConfig_Sight>(FName("Sight Config"));

	Sight->SightRadius = 1000.f;
	Sight->LoseSightRadius = 1500.f;
	Sight->PeripheralVisionAngleDegrees = 130.f;

	//Tell the sight sense to detect everything
	Sight->DetectionByAffiliation.bDetectEnemies = true;
	Sight->DetectionByAffiliation.bDetectFriendlies = true;
	Sight->DetectionByAffiliation.bDetectNeutrals = true;

	//Register the sight sense to our Perception Component
	AIPerceptionComponent->ConfigureSense(*Sight);
}

void AMyAIController::Possess(APawn* InPawn)
{
	Super::Possess(InPawn);

	if (BehaviorTree)
	{
		//Initialize the Blackboard and start the attached behavior tree
		BlackboardComp->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
		BehaviorTreeComp->StartTree(*BehaviorTree);
	}

	//Register the OnPerceptionUpdated function to fire whenever the AIPerception get's updated
	AIPerceptionComponent->OnPerceptionUpdated.AddDynamic(this, &AMyAIController::OnPerceptionUpdated);
}

AActor* AMyAIController::GetSeeingPawn()
{
	//Return the seeing pawn
	UObject* object = BlackboardComp->GetValueAsObject(BlackboardEnemyKey);

	return object ?  Cast<AActor>(object) : nullptr;
}
