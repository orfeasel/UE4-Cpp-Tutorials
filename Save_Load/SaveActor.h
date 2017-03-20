// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "SaveActor.generated.h"

UCLASS()
class SAVELOADPOST_API ASaveActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASaveActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	friend FArchive& operator<<(FArchive& Ar, ASaveActor& SaveActorRef);

	UPROPERTY(EditAnywhere)
	float Health;
};
