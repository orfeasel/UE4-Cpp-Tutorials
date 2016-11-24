// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "NetworkingTut.h"
#include "Kismet/HeadMountedDisplayFunctionLibrary.h"
#include "NetworkingTutCharacter.h"

//////////////////////////////////////////////////////////////////////////
// ANetworkingTutCharacter

ANetworkingTutCharacter::ANetworkingTutCharacter()
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

	//Init. text render comp
	CharText = CreateDefaultSubobject<UTextRenderComponent>(FName("CharText"));

	//Set a relative location
	CharText->SetRelativeLocation(FVector(0, 0, 100));

	//Attach it to root comp
	CharText->SetupAttachment(GetRootComponent());

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

//////////////////////////////////////////////////////////////////////////
// Input

void ANetworkingTutCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &ANetworkingTutCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ANetworkingTutCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ANetworkingTutCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ANetworkingTutCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ANetworkingTutCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ANetworkingTutCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ANetworkingTutCharacter::OnResetVR);


	PlayerInputComponent->BindAction("ThrowBomb", IE_Pressed, this, &ANetworkingTutCharacter::AttempToSpawnBomb);
}


void ANetworkingTutCharacter::OnRep_Health()
{
	UpdateCharText();
}

void ANetworkingTutCharacter::OnRep_BombCount()
{
	UpdateCharText();
}

void ANetworkingTutCharacter::InitHealth()
{
	Health = MaxHealth;
	UpdateCharText();
}

void ANetworkingTutCharacter::InitBombCount()
{
	BombCount = MaxBombCount;
	UpdateCharText();
}

void ANetworkingTutCharacter::UpdateCharText()
{
	//Create a string that will display the health and bomb count values
	FString NewText = FString("Health: ") + FString::SanitizeFloat(Health) + FString(" Bomb Count: ") + FString::FromInt(BombCount);

	//Set the created string to the text render comp
	CharText->SetText(FText::FromString(NewText));
}

void ANetworkingTutCharacter::AttempToSpawnBomb()
{
	if (HasBombs())
	{
		if (Role < ROLE_Authority)
		{
			ServerSpawnBomb();
		}
		else SpawnBomb();
	}
}

void ANetworkingTutCharacter::ServerSpawnBomb_Implementation()
{
	SpawnBomb();
}

bool ANetworkingTutCharacter::ServerSpawnBomb_Validate()
{
	return true;
}

void ANetworkingTutCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//Tell the engine to call the OnRep_Health and OnRep_BombCount each time
	//a variable changes
	DOREPLIFETIME(ANetworkingTutCharacter, Health);
	DOREPLIFETIME(ANetworkingTutCharacter, BombCount);
}

void ANetworkingTutCharacter::BeginPlay()
{
	Super::BeginPlay();

	InitHealth();
	InitBombCount();
}

float ANetworkingTutCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	//Decrease the character's hp 
	
	Health -= Damage;
	if (Health <= 0) InitHealth();

	//Call the update text on the local client
	//OnRep_Health will be called in every other client so the character's text
	//will contain a text with the right values
	UpdateCharText();

	return Health;
}

void ANetworkingTutCharacter::ServerTakeDamage_Implementation(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
}

bool ANetworkingTutCharacter::ServerTakeDamage_Validate(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	//Assume that everything is ok without any further checks and return true
	return true;
}

void ANetworkingTutCharacter::SpawnBomb()
{
	//Decrease the bomb count and update the text in the local client
	//OnRep_BombCount will be called in every other client
	BombCount--;
	UpdateCharText();

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Instigator = this;
	SpawnParameters.Owner = GetController();
	
	//Spawn the bomb
	GetWorld()->SpawnActor<ABomb>(BombActorBP, GetActorLocation() + GetActorForwardVector() * 200, GetActorRotation(),SpawnParameters);
}

#pragma region Mobility Code

void ANetworkingTutCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ANetworkingTutCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void ANetworkingTutCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

void ANetworkingTutCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ANetworkingTutCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ANetworkingTutCharacter::MoveForward(float Value)
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

void ANetworkingTutCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
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

#pragma endregion Auto-generated movement code
