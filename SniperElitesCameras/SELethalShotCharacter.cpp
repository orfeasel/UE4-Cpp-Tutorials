// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "SELethalShot.h"
#include "SELethalShotCharacter.h"
#include "SELethalShotProjectile.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/InputSettings.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

void ASELethalShotCharacter::ActivateThirdPersonCamera()
{
	//Deactivates the first person camera
	FirstPersonCameraComponent->Deactivate();

	//Activates the third person camera
	ThirdPersonCameraComp->Activate();
}

bool ASELethalShotCharacter::HitsAnEnemy(ASELethalShotProjectile* Projectile, ADummyEnemyCharacter*& HitEnemy)
{
	HitEnemy = Cast<ADummyEnemyCharacter>(GetHitActor(Projectile));

	return (HitEnemy) ? true : false;
}

AActor* ASELethalShotCharacter::GetHitActor(ASELethalShotProjectile* Projectile)
{
	FHitResult HitResult;

	FVector StartLocation = Projectile->GetActorLocation();

	FVector EndLocation = StartLocation + (Projectile->GetActorForwardVector() * RaycastLength);

	FCollisionQueryParams CollisionParams;

	//Ignore the character and the projectile
	CollisionParams.AddIgnoredActor(this);
	CollisionParams.AddIgnoredActor(Projectile);

	//Perform a raycast - search for pawns only
	GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECollisionChannel::ECC_Pawn, CollisionParams);

	return HitResult.GetActor();
}

void ASELethalShotCharacter::ActivateProjectileCamera(ASELethalShotProjectile* Projectile)
{
	//Change the active camera based on the assigned blend time from within our editor
	Cast<APlayerController>(GetController())->SetViewTargetWithBlend(Projectile, ThirdPersonCameraToProjectileCameraBlendTime);
}

void ASELethalShotCharacter::ResetActiveCamera()
{
	//Activate the first person camera
	FirstPersonCameraComponent->Activate();
	ThirdPersonCameraComp->Deactivate();

	//Transition to the first person camera
	Cast<APlayerController>(GetController())->SetViewTarget(this);
}

//////////////////////////////////////////////////////////////////////////
// ASELethalShotCharacter

ASELethalShotCharacter::ASELethalShotCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->RelativeLocation = FVector(-39.56f, 1.75f, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(false);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->RelativeRotation = FRotator(1.9f, -19.19f, 5.2f);
	Mesh1P->RelativeLocation = FVector(-0.5f, -4.4f, -155.7f);

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(false);			// only the owning player will see this mesh
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 30.0f, 10.0f);

	//Create the spring arm comp
	ThirdPersonSpringArmComp = CreateDefaultSubobject<USpringArmComponent>(FName("ThirdPersonSpringArmComp"));

	//Attach it to the character's root component
	ThirdPersonSpringArmComp->SetupAttachment(GetRootComponent());

	//Create the third person camera comp
	ThirdPersonCameraComp = CreateDefaultSubobject<UCameraComponent>(FName("ThirdPersonCameraComp"));

	//Attach it our spring arm
	ThirdPersonCameraComp->SetupAttachment(ThirdPersonSpringArmComp);

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P are set in the
	// derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

void ASELethalShotCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint")); //Attach gun mesh component to Skeleton, doing it here because the skelton is not yet created in the constructor

	//Tells the controller to search for an owned camera componenet to view through when used as a view target
	GetController()->bFindCameraComponentWhenViewTarget = true;
}

//////////////////////////////////////////////////////////////////////////
// Input

void ASELethalShotCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// set up gameplay key bindings
	check(InputComponent);

	InputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	//InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &ASELethalShotCharacter::TouchStarted);
	if (EnableTouchscreenMovement(InputComponent) == false)
	{
		InputComponent->BindAction("Fire", IE_Pressed, this, &ASELethalShotCharacter::OnFire);
	}

	InputComponent->BindAxis("MoveForward", this, &ASELethalShotCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ASELethalShotCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &ASELethalShotCharacter::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &ASELethalShotCharacter::LookUpAtRate);
}

void ASELethalShotCharacter::OnFire()
{
	// try and fire a projectile
	if (ProjectileClass != NULL)
	{
		const FRotator SpawnRotation = GetControlRotation();
		// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
		const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

		UWorld* const World = GetWorld();
		if (World != NULL)
		{
			// spawn the projectile at the muzzle
			ASELethalShotProjectile* SpawnedProjectile = World->SpawnActor<ASELethalShotProjectile>(ProjectileClass, SpawnLocation, SpawnRotation);

			ADummyEnemyCharacter* EnemyToBeKilled = nullptr;

			//If the projectile is going to hit an enemy - dilate the time and assign the enemy that we're going to kill
			if (HitsAnEnemy(SpawnedProjectile, EnemyToBeKilled))
			{
				SpawnedProjectile->SetEnemyToKill(EnemyToBeKilled);

				//Dilate the time
				UGameplayStatics::SetGlobalTimeDilation(World, TimeDilationMultiplier);

				//Change the activate camera
				ActivateThirdPersonCamera();

				//Create a timer handle and a timer delegate
				FTimerHandle TimerHandle;
				FTimerDelegate TimerDel;

				//Assign the corresponding UFUNCTION to the timer delegate
				TimerDel.BindUFunction(this, FName("ActivateProjectileCamera"), SpawnedProjectile);

				//Fire the delegate after she specified delay
				World->GetTimerManager().SetTimer(TimerHandle, TimerDel, ThirdPersonToProjectileTransitionDelay, false);
			}
		}
	}

	// try and play the sound if specified
	if (FireSound != NULL)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	// try and play a firing animation if specified
	if (FireAnimation != NULL)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
		if (AnimInstance != NULL)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}

void ASELethalShotCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void ASELethalShotCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		OnFire();
	}
	TouchItem.bIsPressed = false;
}

void ASELethalShotCharacter::TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if ((TouchItem.bIsPressed == true) && (TouchItem.FingerIndex == FingerIndex))
	{
		if (TouchItem.bIsPressed)
		{
			if (GetWorld() != nullptr)
			{
				UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
				if (ViewportClient != nullptr)
				{
					FVector MoveDelta = Location - TouchItem.Location;
					FVector2D ScreenSize;
					ViewportClient->GetViewportSize(ScreenSize);
					FVector2D ScaledDelta = FVector2D(MoveDelta.X, MoveDelta.Y) / ScreenSize;
					if (FMath::Abs(ScaledDelta.X) >= 4.0 / ScreenSize.X)
					{
						TouchItem.bMoved = true;
						float Value = ScaledDelta.X * BaseTurnRate;
						AddControllerYawInput(Value);
					}
					if (FMath::Abs(ScaledDelta.Y) >= 4.0 / ScreenSize.Y)
					{
						TouchItem.bMoved = true;
						float Value = ScaledDelta.Y * BaseTurnRate;
						AddControllerPitchInput(Value);
					}
					TouchItem.Location = Location;
				}
				TouchItem.Location = Location;
			}
		}
	}
}

void ASELethalShotCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ASELethalShotCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void ASELethalShotCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ASELethalShotCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

bool ASELethalShotCharacter::EnableTouchscreenMovement(class UInputComponent* InputComponent)
{
	bool bResult = false;
	if (FPlatformMisc::GetUseVirtualJoysticks() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		bResult = true;
		InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &ASELethalShotCharacter::BeginTouch);
		InputComponent->BindTouch(EInputEvent::IE_Released, this, &ASELethalShotCharacter::EndTouch);
		InputComponent->BindTouch(EInputEvent::IE_Repeat, this, &ASELethalShotCharacter::TouchUpdate);
	}
	return bResult;
}
