// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "InventorySystem.h"
#include "InventorySystemCharacter.h"
#include "InventorySystemProjectile.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/InputSettings.h"
#include "MyPlayerController.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AInventorySystemCharacter
void AInventorySystemCharacter::Raycast()
{
	//Calculating start and end location
	FVector StartLocation = FirstPersonCameraComponent->GetComponentLocation();
	FVector EndLocation = StartLocation + (FirstPersonCameraComponent->GetForwardVector() * RaycastRange);

	FHitResult RaycastHit;

	//Raycast should ignore the character
	FCollisionQueryParams CQP;
	CQP.AddIgnoredActor(this);

	//Raycast
	GetWorld()->LineTraceSingleByChannel(RaycastHit, StartLocation, EndLocation, ECollisionChannel::ECC_WorldDynamic, CQP);


	APickup* Pickup = Cast<APickup>(RaycastHit.GetActor());

	if (LastItemSeen && LastItemSeen != Pickup)
	{
		//If our character sees a different pickup then disable the glowing effect on the previous seen item
		LastItemSeen->SetGlowEffect(false);
	}

	if (Pickup)
	{
		//Enable the glow effect on the current item
		LastItemSeen = Pickup;
		Pickup->SetGlowEffect(true);
	}//Re-Initialize 
	else LastItemSeen = nullptr;

}

void AInventorySystemCharacter::BeginPlay()
{
	Super::BeginPlay();

	//Initializing our reference
	LastItemSeen = nullptr;

	Inventory.SetNum(MAX_INVENTORY_ITEMS);
}

void AInventorySystemCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	//Raycast every frame
	Raycast();
}

AInventorySystemCharacter::AInventorySystemCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->AttachParent = GetCapsuleComponent();
	FirstPersonCameraComponent->RelativeLocation = FVector(0, 0, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->AttachParent = FirstPersonCameraComponent;
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	FP_Gun->AttachTo(Mesh1P, TEXT("GripPoint"), EAttachLocation::SnapToTargetIncludingScale, true);


	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 30.0f, 10.0f);

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P are set in the
	// derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

void AInventorySystemCharacter::PickupItem()
{
	if (LastItemSeen)
	{
		//Find the first available slot
		int32 AvailableSlot = Inventory.Find(nullptr);

		if (AvailableSlot != INDEX_NONE)
		{
			//Add the item to the first valid slot we found
			Inventory[AvailableSlot] = LastItemSeen;
			//Destroy the item from the game
			LastItemSeen->Destroy();
		}
		else GLog->Log("You can't carry any more items!");
	}
}

void AInventorySystemCharacter::DropEquippedItem()
{
	if (CurrentlyEquippedItem)
	{
		int32 IndexOfItem;
		if (Inventory.Find(CurrentlyEquippedItem, IndexOfItem))
		{
			//The location of the drop
			FVector DropLocation = GetActorLocation() + (GetActorForwardVector() * 200);

			//Making a transform with default rotation and scale. Just setting up the location
			//that was calculated above
			FTransform Transform; Transform.SetLocation(DropLocation);

			//Default actor spawn parameters
			FActorSpawnParameters SpawnParams;

			//Spawning our pickup
			APickup* PickupToSpawn = GetWorld()->SpawnActor<APickup>(CurrentlyEquippedItem->GetClass(), Transform, SpawnParams);


			if (PickupToSpawn)
			{
				//Unreference the item we've just placed
				Inventory[IndexOfItem] = nullptr;
			}
		}
	}
}

void AInventorySystemCharacter::SetEquippedItem(UTexture2D * Texture)
{
	if (Texture)
	{
		//For this scenario we make the assumption that
		//every pickup has a unique texture.
		//So, in order to set the equipped item we just check every item
		//inside our Inventory. Once we find an item that has the same image as the
		//Texture image we're passing as a parameter we mark that item as CurrentlyEquipped.
		for (auto It = Inventory.CreateIterator(); It; It++)
		{
			if ((*It) && (*It)->GetPickupTexture() && (*It)->GetPickupTexture()->HasSameSourceArt(Texture))
			{
				CurrentlyEquippedItem = *It;
				GLog->Log("I've set a new equipped item: " + CurrentlyEquippedItem->GetName());
				break;
			}
		}
	}
	else GLog->Log("The Player has clicked an empty inventory slot");
}

//////////////////////////////////////////////////////////////////////////
// Input

void AInventorySystemCharacter::HandleInventoryInput()
{
	AMyPlayerController* Con = Cast<AMyPlayerController>(GetController());
	if (Con) Con->HandleInventoryInput();
}

void AInventorySystemCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// set up gameplay key bindings
	check(InputComponent);

	//Action mapping of pickup item
	InputComponent->BindAction("Pickup", IE_Pressed, this, &AInventorySystemCharacter::PickupItem);

	FInputActionBinding InventoryBinding;
	//We need this bind to execute on pause state
	InventoryBinding.bExecuteWhenPaused = true;
	InventoryBinding.ActionDelegate.BindDelegate(this, FName("HandleInventoryInput"));
	InventoryBinding.ActionName = FName("Inventory");
	InventoryBinding.KeyEvent = IE_Pressed;

	//Binding the Inventory action
	InputComponent->AddActionBinding(InventoryBinding);

	InputComponent->BindAction("DropItem", IE_Pressed, this, &AInventorySystemCharacter::DropEquippedItem);

	//InputComponent->BindAction("Inventory", IE_Pressed, this, &AInventorySystemCharacter::HandleInventoryInput);

	InputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	//InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AInventorySystemCharacter::TouchStarted);
	if (EnableTouchscreenMovement(InputComponent) == false)
	{
		InputComponent->BindAction("Fire", IE_Pressed, this, &AInventorySystemCharacter::OnFire);
	}

	InputComponent->BindAxis("MoveForward", this, &AInventorySystemCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AInventorySystemCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &AInventorySystemCharacter::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &AInventorySystemCharacter::LookUpAtRate);
}

void AInventorySystemCharacter::OnFire()
{
	// try and fire a projectile
	if (ProjectileClass != NULL)
	{
		const FRotator SpawnRotation = GetControlRotation();
		// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
		const FVector SpawnLocation = GetActorLocation() + SpawnRotation.RotateVector(GunOffset);

		UWorld* const World = GetWorld();
		if (World != NULL)
		{
			// spawn the projectile at the muzzle
			World->SpawnActor<AInventorySystemProjectile>(ProjectileClass, SpawnLocation, SpawnRotation);
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

void AInventorySystemCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
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

void AInventorySystemCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
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

void AInventorySystemCharacter::TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location)
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
					if (ScaledDelta.X != 0.0f)
					{
						TouchItem.bMoved = true;
						float Value = ScaledDelta.X * BaseTurnRate;
						AddControllerYawInput(Value);
					}
					if (ScaledDelta.Y != 0.0f)
					{
						TouchItem.bMoved = true;
						float Value = ScaledDelta.Y* BaseTurnRate;
						AddControllerPitchInput(Value);
					}
					TouchItem.Location = Location;
				}
				TouchItem.Location = Location;
			}
		}
	}
}

void AInventorySystemCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AInventorySystemCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AInventorySystemCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AInventorySystemCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

bool AInventorySystemCharacter::EnableTouchscreenMovement(class UInputComponent* InputComponent)
{
	bool bResult = false;
	if (FPlatformMisc::GetUseVirtualJoysticks() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		bResult = true;
		InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AInventorySystemCharacter::BeginTouch);
		InputComponent->BindTouch(EInputEvent::IE_Released, this, &AInventorySystemCharacter::EndTouch);
		InputComponent->BindTouch(EInputEvent::IE_Repeat, this, &AInventorySystemCharacter::TouchUpdate);
	}
	return bResult;
}
