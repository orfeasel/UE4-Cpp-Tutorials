// Fill out your copyright notice in the Description page of Project Settings.

#include "DialogSystem.h"
#include "AICharacter.h"
#include "DialogSystemCharacter.h"

// Sets default values
AAICharacter::AAICharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxComp = CreateDefaultSubobject<UBoxComponent>(FName("BoxComp"));
	BoxComp->AttachTo(GetRootComponent());

	AudioComp = CreateDefaultSubobject<UAudioComponent>(FName("AudioComp"));
	AudioComp->AttachTo(GetRootComponent());

}

// Called when the game starts or when spawned
void AAICharacter::BeginPlay()
{
	Super::BeginPlay();

	//Register the begin and end overlap functions
	BoxComp->OnComponentBeginOverlap.AddDynamic(this, &AAICharacter::OnBoxOverlap);
	BoxComp->OnComponentEndOverlap.AddDynamic(this, &AAICharacter::OnBoxEndOverlap);
	
}

void AAICharacter::OnBoxOverlap(AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor->IsA<ADialogSystemCharacter>())
	{
		ADialogSystemCharacter* Char = Cast<ADialogSystemCharacter>(OtherActor);
		Char->SetTalkRangeStatus(true);
		Char->GeneratePlayerLines(*PlayerLines);
		Char->SetAssociatedPawn(this);
	}
}

void AAICharacter::OnBoxEndOverlap(AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherIndex)
{
	if (OtherActor->IsA<ADialogSystemCharacter>())
	{
		ADialogSystemCharacter* Char = Cast<ADialogSystemCharacter>(OtherActor);
		Char->SetTalkRangeStatus(false);
		Char->SetAssociatedPawn(nullptr);
	}
}

void AAICharacter::Talk(USoundBase * SFX, TArray<FSubtitle> Subs)
{
	ADialogSystemCharacter* Char = Cast<ADialogSystemCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	
	//Play the corresponding sfx
	AudioComp->SetSound(SFX);
	AudioComp->Play();

	//Tell the UI to update with the new subtitles
	Char->GetUI()->UpdateSubtitles(Subs);
}

void AAICharacter::AnswerToCharacter(FName PlayerLine, TArray<FSubtitle>& SubtitlesToDisplay, float delay)
{
	if (!AILines) return;

	//Retrieve the corresponding line
	FString ContextString;
	FDialog* Dialog = AILines->FindRow<FDialog>(PlayerLine, ContextString);

	ADialogSystemCharacter* MainChar = Cast<ADialogSystemCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	if (Dialog && MainChar)
	{
		FTimerHandle TimerHandle;
		FTimerDelegate TimerDel;

		TimerDel.BindUFunction(this, FName("Talk"), Dialog->SFX, Dialog->Subtitles);

		//Talk to the player after the delay time has passed
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDel, delay, false);
	}
}
