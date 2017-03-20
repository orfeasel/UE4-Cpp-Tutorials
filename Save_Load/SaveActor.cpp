// Fill out your copyright notice in the Description page of Project Settings.

#include "SaveLoadPost.h"
#include "SaveActor.h"


// Sets default values
ASaveActor::ASaveActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASaveActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASaveActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FArchive & operator<<(FArchive & Ar, ASaveActor & SaveActorRef)
{
	Ar << SaveActorRef.Health;

	return Ar;
}
