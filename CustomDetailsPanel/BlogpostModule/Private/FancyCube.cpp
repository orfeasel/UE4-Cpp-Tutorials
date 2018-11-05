// Fill out your copyright notice in the Description page of Project Settings.

#include "FancyCube.h"


// Sets default values
AFancyCube::AFancyCube()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CubeSM = CreateDefaultSubobject<UStaticMeshComponent>("CubeSM");
}

void AFancyCube::AssignColor(FLinearColor NewColor)
{
	if (CubeSM)
	{
		UMaterialInstanceDynamic* MID = CubeSM->CreateAndSetMaterialInstanceDynamic(0);
		MID->SetVectorParameterValue("BaseColor", NewColor);
	}
	
}

// Called when the game starts or when spawned
void AFancyCube::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFancyCube::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

