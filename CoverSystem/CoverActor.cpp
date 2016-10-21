// Fill out your copyright notice in the Description page of Project Settings.

#include "CoverSystem.h"
#include "CoverActor.h"
#include "CoverSystemCharacter.h"

void ACoverActor::OnCompBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA<ACoverSystemCharacter>())
	{
		//Inform the player that he is able to take cover
		ACoverSystemCharacter* Char = Cast<ACoverSystemCharacter>(OtherActor);
		Char->SetCanTakeCover(true, this);
	}
}

void ACoverActor::OnCompEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->IsA<ACoverSystemCharacter>())
	{
		//Inform the player that he isn't able to take cover
		ACoverSystemCharacter* Char = Cast<ACoverSystemCharacter>(OtherActor);
		Char->SetCanTakeCover(false,nullptr);
	}
}

bool ACoverActor::IsCloseToPlayer(FName SocketName)
{
	//Perform a raycast in order to determine if the player is 
	//near the given socket
	TArray<FHitResult> HitResults;

	const FVector StartLocation = SM->GetSocketLocation(SocketName);

	const FVector EndLocation = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->GetActorLocation();

	FCollisionShape Shape;
	Shape.ShapeType = ECollisionShape::Line;

	GetWorld()->SweepMultiByChannel(HitResults, StartLocation, EndLocation, FQuat(), ECC_WorldDynamic, Shape);

	//If our raycast contains the character inside its hit result
	//the character can take cover in the side that this socket represents
	return HitResults.ContainsByPredicate([&](FHitResult hitResult)
	{
		AActor* HitActor = hitResult.GetActor();
		return HitActor && HitActor->IsA<ACoverSystemCharacter>();
	});
}

FName ACoverActor::GetNearbySocket()
{
	const FName AvailableSockets[4] =
	{
		FName("ForwardSocket"),
		FName("BackwardSocket"),
		FName("RightSocket"),
		FName("LeftSocket")
	};

	//Find the socket that is close to the character
	for (uint8 SocketPtr = 0; SocketPtr < 4; SocketPtr++)
	{
		if (IsCloseToPlayer(AvailableSockets[SocketPtr])) return AvailableSockets[SocketPtr];
	}

	//If something goes terribly wrong we're going to get the forward wall
	return AvailableSockets[0];
}

void ACoverActor::DetermineMovementDirection(FVector& MovementDirection, FRotator& FacingDirection)
{
	FName NearbySocket = GetNearbySocket();
	
	AActor* Char = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	//Determine the movement and facing direction of the player, based on the described logic
	//The way that we're deciding the facing direction is similar to the way we've decided
	//the movement direction
	if (NearbySocket.IsEqual("ForwardSocket"))
	{
		MovementDirection = -GetActorRightVector();
		FacingDirection = GetActorRotation();
	}
	else if (NearbySocket.IsEqual("BackwardSocket"))
	{
		MovementDirection = GetActorRightVector();
		FacingDirection = GetActorRotation() + FRotator(0, 180, 0);
	}
	else if (NearbySocket.IsEqual("RightSocket"))
	{
		MovementDirection = GetActorForwardVector();
		FacingDirection = GetActorRotation() + FRotator(0, 90, 0);
	}
	else
	{
		MovementDirection = -GetActorForwardVector();
		FacingDirection = GetActorRotation() + FRotator(0, -90.f, 0);
	}
}

// Sets default values
ACoverActor::ACoverActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Init. components
	SM = CreateDefaultSubobject<UStaticMeshComponent>(FName("SM"));
	BoxComp = CreateDefaultSubobject<UBoxComponent>(FName("BoxComp"));
	
	SetRootComponent(SM);

	BoxComp->SetupAttachment(SM);
}

// Called when the game starts or when spawned
void ACoverActor::BeginPlay()
{
	Super::BeginPlay();

	if (BoxComp)
	{
		//Register overlap events
		BoxComp->OnComponentBeginOverlap.AddDynamic(this, &ACoverActor::OnCompBeginOverlap);
		BoxComp->OnComponentEndOverlap.AddDynamic(this, &ACoverActor::OnCompEndOverlap);
	}
}

void ACoverActor::RetrieveMovementDirectionAndFacingRotation(FVector& MovementDirection, FRotator& FacingRotation)
{
	DetermineMovementDirection(MovementDirection, FacingRotation);
}

