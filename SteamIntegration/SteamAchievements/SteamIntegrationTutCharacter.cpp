// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "SteamIntegrationTut.h"
#include "Kismet/HeadMountedDisplayFunctionLibrary.h"
#include "SteamIntegrationTutCharacter.h"

#include "OnlineAchievementsInterface.h"
#include "OnlineIdentityInterface.h"
#include "OnlineSubsystem.h"
#include "Engine/LocalPlayer.h"


#define ACH_WIN_ONE_GAME FString("ACH_WIN_ONE_GAME")
#define AC_WIN_100_GAMES FString("ACH_WIN_100_GAMES")
#define AC_TRAVEL_FAR_SINGLE FString("ACH_TRAVEL_FAR_SINGLE")

//////////////////////////////////////////////////////////////////////////
// ASteamIntegrationTutCharacter

ASteamIntegrationTutCharacter::ASteamIntegrationTutCharacter()
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
}

void ASteamIntegrationTutCharacter::BeginPlay()
{
	Super::BeginPlay();

	//Cache all the available achievements
	QueryAchievements();
}

void ASteamIntegrationTutCharacter::QueryAchievements()
{
	//Get the online sub system
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();

	if (OnlineSub)
	{
		//Get the Identity from the sub system 
		//Meaning our player's profile and various online services
		IOnlineIdentityPtr Identity = OnlineSub->GetIdentityInterface();

		if (Identity.IsValid())
		{
			//Get a thread-safe pointer (for more info check out this link: https://docs.unrealengine.com/latest/INT/API/Runtime/Core/Templates/TSharedPtr/index.html )
			TSharedPtr<const FUniqueNetId> UserId = Identity->GetUniquePlayerId(0);

			//Get the achievements interface for this platform
			IOnlineAchievementsPtr Achievements = OnlineSub->GetAchievementsInterface();

			if (Achievements.IsValid())
			{
				//Cache all the game's achievements for future use and bind the OnQueryAchievementsComplete function to fire when we're finished caching
				Achievements->QueryAchievements(*UserId.Get(), FOnQueryAchievementsCompleteDelegate::CreateUObject(this, &ASteamIntegrationTutCharacter::OnQueryAchievementsComplete));
			}
		}
	}
}

void ASteamIntegrationTutCharacter::OnQueryAchievementsComplete(const FUniqueNetId& PlayerId, const bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		GLog->Log("achievements were cached");
	}
	else
	{
		GLog->Log("failed to cache achievements");
	}
}

void ASteamIntegrationTutCharacter::UpdateAchievementProgress(const FString& Id, float Percent)
{
	//Get the online sub system
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();

	if (OnlineSub)
	{
		//Get the Identity from the sub system 
		//Meaning our player's profile and various online services
		IOnlineIdentityPtr Identity = OnlineSub->GetIdentityInterface();

		if (Identity.IsValid())
		{
			//Get a thread-safe pointer (for more info check out this link: https://docs.unrealengine.com/latest/INT/API/Runtime/Core/Templates/TSharedPtr/index.html )
			TSharedPtr<const FUniqueNetId> UserId = Identity->GetUniquePlayerId(0);

			//Get the achievements interface for this platform
			IOnlineAchievementsPtr Achievements = OnlineSub->GetAchievementsInterface();

			
			if (Achievements.IsValid() && (!AchievementsWriteObjectPtr.IsValid() || !AchievementsWriteObjectPtr->WriteState != EOnlineAsyncTaskState::InProgress))
			{
				//Make a shared pointer for achievement writing
				AchievementsWriteObjectPtr = MakeShareable(new FOnlineAchievementsWrite());

				//Sets the progress of the desired achievement - does nothing if the id is not valid
				AchievementsWriteObjectPtr->SetFloatStat(*Id, Percent);

				//Write the achievements progress
				FOnlineAchievementsWriteRef AchievementsWriteObjectRef = AchievementsWriteObjectPtr.ToSharedRef();
				Achievements->WriteAchievements(*UserId, AchievementsWriteObjectRef);
			}
		}
	}
}

void ASteamIntegrationTutCharacter::WinGameAchievement()
{
	UpdateAchievementProgress(ACH_WIN_ONE_GAME, 100.f);
}

void ASteamIntegrationTutCharacter::WinManyGamesAchievement()
{
	UpdateAchievementProgress(AC_WIN_100_GAMES, 100.f);
}

void ASteamIntegrationTutCharacter::TravelFarAwayAchievement()
{
	UpdateAchievementProgress(AC_TRAVEL_FAR_SINGLE, 100.f);
}

//////////////////////////////////////////////////////////////////////////
// Input

void ASteamIntegrationTutCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASteamIntegrationTutCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASteamIntegrationTutCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ASteamIntegrationTutCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ASteamIntegrationTutCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ASteamIntegrationTutCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ASteamIntegrationTutCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ASteamIntegrationTutCharacter::OnResetVR);
}


void ASteamIntegrationTutCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ASteamIntegrationTutCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void ASteamIntegrationTutCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

void ASteamIntegrationTutCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ASteamIntegrationTutCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ASteamIntegrationTutCharacter::MoveForward(float Value)
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

void ASteamIntegrationTutCharacter::MoveRight(float Value)
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
