// Fill out your copyright notice in the Description page of Project Settings.

#include "SELethalShot.h"
#include "DummyEnemyCharacter.h"
#include "SELethalShotCharacter.h"

void ADummyEnemyCharacter::EnableCameraTransition()
{
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetViewTargetWithBlend(this, DeathCameraBlendTime);
}

void ADummyEnemyCharacter::Die()
{
	USkeletalMeshComponent* CharSM = GetMesh();
	//Enable ragdoll physics for our dummy enemy (to simulate a dying effect)
	CharSM->SetSimulatePhysics(true);
	CharSM->SetAllBodiesSimulatePhysics(true);
	CharSM->WakeAllRigidBodies();
	CharSM->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

	//Reset the global time to default
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.f);

	FTimerHandle TimerHandle;
	FTimerDelegate TimerDel;

	TimerDel.BindLambda([&]()
	{
		//Deactive the death camera
		DeathCameraComp->Deactivate();

		ASELethalShotCharacter* MainChar = Cast<ASELethalShotCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
		MainChar->ResetActiveCamera();
	});
	
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDel, CameraResetDelay, false);
}

// Sets default values
ADummyEnemyCharacter::ADummyEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DeathSpringArmComp = CreateDefaultSubobject<USpringArmComponent>(FName("DeathSpringArmComp"));

	DeathSpringArmComp->SetupAttachment(GetRootComponent());

	DeathCameraComp = CreateDefaultSubobject<UCameraComponent>(FName("DeathCameraComp"));

	DeathCameraComp->SetupAttachment(DeathSpringArmComp);
}

