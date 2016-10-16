// Fill out your copyright notice in the Description page of Project Settings.

#include "EqsTut.h"
#include "EnvQueryGenerator_Cone.h"




void UEnvQueryGenerator_Cone::GenerateItems(FEnvQueryInstance& QueryInstance) const
{
	//This array will hold a reference to all the generated items, meaning, the cone items
	TArray<FNavLocation> ItemCandidates;

	//Get a reference for our AI Pawn
	AActor* AIPawn = Cast<AActor>((QueryInstance.Owner).Get());

	//Store its location and its forward vector
	FVector PawnLocation = AIPawn->GetActorLocation();
	FVector PawnForwardVector = AIPawn->GetActorForwardVector();

	//If the angle step is zero we're going into an infinite loop. 
	//Since we don't want that, don't execute the following logic
	if (AngleStep == 0) return;

	for (float Angle = -ConeDegrees; Angle < ConeDegrees; Angle += AngleStep)
	{
		//Start from the left side of the pawn and rotate its forward vector by Angle + 1
		FVector LeftVector = PawnForwardVector.RotateAngleAxis(Angle + 1, FVector(0, 0, 1));
		//The Left Vector is showing a straight line for that angle. The only thing we need
		//is to generate items in that line

		//Generates all the points for the current line (LeftVector)
		for (int32 Point = 0; Point < ConeRadius; Point++)
		{
			//Generate a point for this particular angle and distance
			FNavLocation NavLoc = FNavLocation(PawnLocation + LeftVector * Point * PointsDistance);

			//Add the new point into our array
			ItemCandidates.Add(NavLoc);
		}
	}

	//Projects all the nav points into our Viewport and removes those outside of our navmesh
	ProjectAndFilterNavPoints(ItemCandidates, QueryInstance);

	//Store the generated points as the result of our Query
	StoreNavPoints(ItemCandidates, QueryInstance);
}
