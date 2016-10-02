// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "FlightStop.generated.h"

UCLASS()
class WOWFLIGHT_API AFlightStop : public AActor
{
	GENERATED_BODY()

protected:

	/** The FloatCurve corresponding to the previous flight spline component */
	UPROPERTY(EditAnywhere)
	UCurveFloat* PreviousFlightCurve;

	/** The FloatCurve corresponding to the next flight spline component */
	UPROPERTY(EditAnywhere)
	UCurveFloat* NextFlightCurve;

	/** A static mesh for our flight stop */
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* SM;

	/** The spline component that describes the flight path of the next flight */
	UPROPERTY(VisibleAnywhere)
	USplineComponent* NextFlightStop;

	/** The spline component that describes the flight path of the previous flight */
	UPROPERTY(VisibleAnywhere)
	USplineComponent* PreviousFlightStop;
	
public:	
	// Sets default values for this actor's properties
	AFlightStop();

	/** Returns the previous flight curve */
	UCurveFloat* GetPreviousFlightCurve() { return PreviousFlightCurve; };

	/** Returns the next flight curve */
	UCurveFloat* GetNextFlightCurve() { return NextFlightCurve; };

	/** Returns the next flight spline component */
	USplineComponent* GetNextFlightSplineComp() { return NextFlightStop; };

	/**Returns the previous flight spline component */
	USplineComponent* GetPreviousFlightSplineComp() { return PreviousFlightStop; };
};
