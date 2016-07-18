// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "TLSkillsTree.h"
#include "TLSkillsTreeCharacter.h"
#include "Skill.h"

//////////////////////////////////////////////////////////////////////////
// ATLSkillsTreeCharacter

ATLSkillsTreeCharacter::ATLSkillsTreeCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	//Create the root component for our spring arms
	SkillsRootComp = CreateDefaultSubobject<USceneComponent>(FName("SkillsRootComp"));

	//Attach it to our root
	SkillsRootComp->SetupAttachment(RootComponent);

	//Create the spring arm components and attach them to their root
	LevelOneSpringArm = CreateDefaultSubobject<USpringArmComponent>(FName("LevelOneSpringArm"));
	LevelTwoSpringArm = CreateDefaultSubobject<USpringArmComponent>(FName("LevelTwoSpringArm"));
	LevelThreeSpringArm = CreateDefaultSubobject<USpringArmComponent>(FName("LevelThreeSpringArm"));


	LevelOneSpringArm->SetupAttachment(SkillsRootComp);
	LevelTwoSpringArm->SetupAttachment(SkillsRootComp);
	LevelThreeSpringArm->SetupAttachment(SkillsRootComp);

	//Initializing the skills component
	SkillsComponent = CreateDefaultSubobject<USkillsComponent>(FName("SkillsComponent"));

}

//////////////////////////////////////////////////////////////////////////
// Input

void ATLSkillsTreeCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// Set up gameplay key bindings
	check(InputComponent);
	InputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	InputComponent->BindAxis("MoveForward", this, &ATLSkillsTreeCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ATLSkillsTreeCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &ATLSkillsTreeCharacter::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &ATLSkillsTreeCharacter::LookUpAtRate);

	// handle touch devices
	InputComponent->BindTouch(IE_Pressed, this, &ATLSkillsTreeCharacter::TouchStarted);
	InputComponent->BindTouch(IE_Released, this, &ATLSkillsTreeCharacter::TouchStopped);
}


FTransform ATLSkillsTreeCharacter::GetFixedSpringArmTransform(USpringArmComponent* SpringArm)
{
	FTransform result;
	if (SpringArm)
	{
		result = SpringArm->GetComponentTransform();
		//We want a fixed location for our transform, since we don't want to spawn our skills
		//right on top of our character.
		result.SetLocation(result.GetLocation() + SpringArm->GetForwardVector() * 100);
	}
	return result;
}

void ATLSkillsTreeCharacter::Fire(bool bShouldFireSecondary)
{
	//This is a dummy logic - we will only have 2 skills for this post
	TSubclassOf<ASkill> SkillBP = (bShouldFireSecondary && SkillsComponent->SkillsArray.IsValidIndex(1)) ? SkillsComponent->SkillsArray[1] : SkillsComponent->SkillsArray[0];

	if (SkillBP)
	{
		FActorSpawnParameters ActorSpawnParams;

		TArray<FTransform> SpawnTransforms = GetSpawnTransforms(SkillBP->GetDefaultObject<ASkill>()->GetLevel());

		for (int32 i = 0; i < SpawnTransforms.Num(); i++)
		{	
			GetWorld()->SpawnActor<ASkill>(SkillBP, SpawnTransforms[i]);
		}

	}
}

TArray<FTransform> ATLSkillsTreeCharacter::GetSpawnTransforms(int32 level)
{
	TArray<FTransform> SpawnPoints;
	switch (level)
	{
		case 1:
		{
			SpawnPoints.Add(GetFixedSpringArmTransform(LevelOneSpringArm));
			break; 
		}
		case 2:
		{
			SpawnPoints.Add(GetFixedSpringArmTransform(LevelTwoSpringArm));
			SpawnPoints.Add(GetFixedSpringArmTransform(LevelThreeSpringArm));
			break;
		}
		case 3:
		{
			SpawnPoints.Add(GetFixedSpringArmTransform(LevelOneSpringArm));
			SpawnPoints.Add(GetFixedSpringArmTransform(LevelTwoSpringArm));
			SpawnPoints.Add(GetFixedSpringArmTransform(LevelThreeSpringArm));
		}
		default:
			break;
	}
	return SpawnPoints;
}


#pragma region DefaultInputs 
void ATLSkillsTreeCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	// jump, but only on the first touch
	if (FingerIndex == ETouchIndex::Touch1)
	{
		Jump();
	}
}

void ATLSkillsTreeCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	if (FingerIndex == ETouchIndex::Touch1)
	{
		StopJumping();
	}
}

void ATLSkillsTreeCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ATLSkillsTreeCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ATLSkillsTreeCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ATLSkillsTreeCharacter::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}
#pragma endregion - The default UE4 generated code for movement and camera controls