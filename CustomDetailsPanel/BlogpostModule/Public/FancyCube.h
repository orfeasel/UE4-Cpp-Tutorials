// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FancyCube.generated.h"

UCLASS()
class BLOGPOSTMODULE_API AFancyCube : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFancyCube();

	void AssignColor(FLinearColor NewColor);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* CubeSM;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
