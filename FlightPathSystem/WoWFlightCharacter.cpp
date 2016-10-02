// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "WoWFlight.h"
#include "Kismet/HeadMountedDisplayFunctionLibrary.h"
#include "WoWFlightCharacter.h"
//////////////////////////////////////////////////////////////////////////
// AWoWFlightCharacter

AWoWFlightCharacter::AWoWFlightCharacter()
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



	FlightBoxCollider = CreateDefaultSubobject<UBoxComponent>(FName("FlightBoxCollider"));

	FlightBoxCollider->SetBoxExtent(FVector(150.f));
	
	FlightBoxCollider->SetupAttachment(GetRootComponent());
}

void AWoWFlightCharacter::BeginPlay()
{
	Super::BeginPlay();

	//Register a function that gets called when the box overlaps with a component
	FlightBoxCollider->OnComponentBeginOverlap.AddDynamic(this, &AWoWFlightCharacter::OnFlightBoxColliderOverlap);
}

void AWoWFlightCharacter::TickTimeline(float Value)
{
	float SplineLength = ActiveSplineComponent->GetSplineLength();

	//Get the new location based on the provided values from the timeline.
	//The reason we're multiplying Value with SplineLength is because all our designed curves in the UE4 editor have a time range of 0 - X.
	//Where X is the total flight time
	FVector NewLocation = ActiveSplineComponent->GetLocationAtDistanceAlongSpline(Value * SplineLength, ESplineCoordinateSpace::World);

	SetActorLocation(NewLocation);

	FRotator NewRotation = ActiveSplineComponent->GetRotationAtDistanceAlongSpline(Value * SplineLength, ESplineCoordinateSpace::World);

	//We're not interested in the pitch value of the above rotation so we make sure to set it to zero
	NewRotation.Pitch = 0;

	SetActorRotation(NewRotation);
}

void AWoWFlightCharacter::OnFlightBoxColliderOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA<AFlightStop>())
	{
		//Store a reference of the nearby flight stop actor
		ActiveFlightStopActor = Cast<AFlightStop>(OtherActor);
	}
}

void AWoWFlightCharacter::NextFlightPathSelected()
{
	if (ActiveFlightStopActor)
	{
		//Get the next flight path's spline component and update the flight timeline with the corresponding curve
		ActiveSplineComponent = ActiveFlightStopActor->GetNextFlightSplineComp();
		UpdateFlightTimeline(ActiveFlightStopActor->GetNextFlightCurve());
	}
}

void AWoWFlightCharacter::PreviousFlightPathSelected()
{
	if (ActiveFlightStopActor)
	{
		//Get the previous flight path's spline component and update the flight timeline with the corresponding curve
		ActiveSplineComponent = ActiveFlightStopActor->GetPreviousFlightSplineComp();
		UpdateFlightTimeline(ActiveFlightStopActor->GetPreviousFlightCurve());
	}
}

void AWoWFlightCharacter::UpdateFlightTimeline(UCurveFloat* CurveFloatToBind)
{
	//Initialize a timeline
	FlightTimeline = FTimeline();

	FOnTimelineFloat ProgressFunction;

	//Bind the function that ticks the timeline
	ProgressFunction.BindUFunction(this, FName("TickTimeline"));

	//Assign the provided curve and progress function for our timeline
	FlightTimeline.AddInterpFloat(CurveFloatToBind, ProgressFunction);
	FlightTimeline.SetLooping(false);
	FlightTimeline.PlayFromStart();

	//Set the timeline's length to match the last key frame based on the given curve
	FlightTimeline.SetTimelineLengthMode(TL_LastKeyFrame);

	//The ResetActiveFlightStopActor executes when the timeline finishes.
	//By calling ResetActiveFlightStopActor at the end of the timeline we make sure to reset any invalid references on ActiveFlightStopActor
	FOnTimelineEvent TimelineEvent;
	TimelineEvent.BindUFunction(this, FName("ResetActiveFlightStopActor"));
	FlightTimeline.SetTimelineFinishedFunc(TimelineEvent);
}

void AWoWFlightCharacter::ResetActiveFlightStopActor()
{
	ActiveFlightStopActor = nullptr;
}

void AWoWFlightCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	//If the timeline has started, advance it by DeltaSeconds
	if (FlightTimeline.IsPlaying()) FlightTimeline.TickTimeline(DeltaSeconds);
}

//////////////////////////////////////////////////////////////////////////
// Input

void AWoWFlightCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &AWoWFlightCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AWoWFlightCharacter::MoveRight);

	//Bind the functions that execute on key press
	PlayerInputComponent->BindAction("NextFlightPath", IE_Pressed, this, &AWoWFlightCharacter::NextFlightPathSelected);
	PlayerInputComponent->BindAction("PreviousFlightPath", IE_Pressed, this, &AWoWFlightCharacter::PreviousFlightPathSelected);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AWoWFlightCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AWoWFlightCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AWoWFlightCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AWoWFlightCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AWoWFlightCharacter::OnResetVR);
}


void AWoWFlightCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AWoWFlightCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	// jump, but only on the first touch
	if (FingerIndex == ETouchIndex::Touch1)
	{
		Jump();
	}
}

void AWoWFlightCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	if (FingerIndex == ETouchIndex::Touch1)
	{
		StopJumping();
	}
}

void AWoWFlightCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AWoWFlightCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AWoWFlightCharacter::MoveForward(float Value)
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

void AWoWFlightCharacter::MoveRight(float Value)
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
