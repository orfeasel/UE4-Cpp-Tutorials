// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "CoverSystem.h"
#include "Kismet/HeadMountedDisplayFunctionLibrary.h"
#include "CoverSystemCharacter.h"
#include "DrawDebugHelpers.h"

void ACoverSystemCharacter::ToggleCrouch()
{
	if (GetCharacterMovement()->IsCrouching())
	{
		//The player is already crouching - disable the crouch effect
		UnCrouch();

		bIsInCover = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
	}
	else
	{
		Crouch();
	}
}

void ACoverSystemCharacter::ToggleCover()
{
	if (GetCharacterMovement()->IsCrouching() && bCanTakeCover)
	{
		bIsInCover = !bIsInCover;

		if (bIsInCover && Cover)
		{
			//This is done because my downloaded animations do not require an orientation to movement
			//Depending on your animation you may (or not) need this
			GetCharacterMovement()->bOrientRotationToMovement = false;

			FRotator CoverRotation;
			Cover->RetrieveMovementDirectionAndFacingRotation(CoverDirectionMovement, CoverRotation);
			SetActorRotation(CoverRotation);
		}
		else
		{
			//This is done because my downloaded animations do not require an orientation to movement
			//Depending on your animation you may (or not) need this
			GetCharacterMovement()->bOrientRotationToMovement = true;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// ACoverSystemCharacter

ACoverSystemCharacter::ACoverSystemCharacter()
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

	//Enable the crouch functionality
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

void ACoverSystemCharacter::SetCanTakeCover(bool CanTakeCover, ACoverActor* CoverActor)
{
	if (!CanTakeCover && bIsInCover)
	{
		ToggleCover();
	}

	bCanTakeCover = CanTakeCover;
	Cover = CoverActor;
}

//////////////////////////////////////////////////////////////////////////
// Input

void ACoverSystemCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &ACoverSystemCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACoverSystemCharacter::MoveRight);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ACoverSystemCharacter::ToggleCrouch);

	PlayerInputComponent->BindAction("TakeCover", IE_Pressed, this, &ACoverSystemCharacter::ToggleCover);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ACoverSystemCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ACoverSystemCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ACoverSystemCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ACoverSystemCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ACoverSystemCharacter::OnResetVR);
}


void ACoverSystemCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ACoverSystemCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	// jump, but only on the first touch
	if (FingerIndex == ETouchIndex::Touch1)
	{
		Jump();
	}
}

void ACoverSystemCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	if (FingerIndex == ETouchIndex::Touch1)
	{
		StopJumping();
	}
}

void ACoverSystemCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ACoverSystemCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ACoverSystemCharacter::MoveForward(float Value)
{
	//We don't want to move forward or backwards when we're taking cover
	if ((Controller != NULL) && (Value != 0.0f) && !bIsInCover)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ACoverSystemCharacter::MoveRight(float Value)
{
	//for the animation bp
	CoverValue = Value;

	if ((Controller != NULL) && (Value != 0.0f))
	{
		if (!bIsInCover)
		{
			//default movement functionality

			// find out which way is right
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);

			// get right vector 
			FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
			AddMovementInput(Direction, Value);
		}
		else
		{
			//Move according to the cover actor's position
			AddMovementInput(CoverDirectionMovement, Value);
		}
	}
}
