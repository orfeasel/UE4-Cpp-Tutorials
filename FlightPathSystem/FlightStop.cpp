// Fill out your copyright notice in the Description page of Project Settings.

#include "WoWFlight.h"
#include "FlightStop.h"


// Sets default values
AFlightStop::AFlightStop()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SM = CreateDefaultSubobject<UStaticMeshComponent>(FName("SM"));
	SetRootComponent(SM);

	//Init splines
	NextFlightStop = CreateDefaultSubobject<USplineComponent>(FName("SplineComp"));
	PreviousFlightStop = CreateDefaultSubobject<USplineComponent>(FName("PreviousFlightStop"));

	//Attach them to root component
	NextFlightStop->SetupAttachment(SM);
	PreviousFlightStop->SetupAttachment(SM);
}
