// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "Minimap.h"
#include "MinimapHUD.h"
#include "Engine/Canvas.h"
#include "TextureResource.h"
#include "CanvasItem.h"

AMinimapHUD::AMinimapHUD()
{
	// Set the crosshair texture
	static ConstructorHelpers::FObjectFinder<UTexture2D> CrosshiarTexObj(TEXT("/Game/FirstPerson/Textures/FirstPersonCrosshair"));
	CrosshairTex = CrosshiarTexObj.Object;
}


void AMinimapHUD::DrawHUD()
{
	//Default template code

	Super::DrawHUD();

	// Draw very simple crosshair

	// find center of the Canvas
	const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);

	// offset by half the texture's dimensions so that the center of the texture aligns with the center of the Canvas
	const FVector2D CrosshairDrawPosition((Center.X),
		(Center.Y));

	// draw the crosshair
	FCanvasTileItem TileItem(CrosshairDrawPosition, CrosshairTex->Resource, FLinearColor::White);
	TileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(TileItem);


	//----------------Radar logic----------------
	
	DrawRadar();
	
	DrawPlayerInRadar();

	PerformRadarRaycast();

	DrawRaycastedActors();

	//Empty the radar actors in case the player moves out of range,
	//by doing so, we have always a valid display in our radar
	RadarActors.Empty();
}

FVector2D AMinimapHUD::GetRadarCenterPosition()
{
	//If the canvas is valid, return the center as a 2d vector
	return (Canvas) ? FVector2D(Canvas->SizeX*RadarStartLocation.X, Canvas->SizeY*RadarStartLocation.Y) : FVector2D(0, 0);
}

FVector2D AMinimapHUD::ConvertWorldLocationToLocal(AActor* ActorToPlace)
{
	APawn* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	if (Player && ActorToPlace)
	{
		//Convert the world location to local, based on the transform of the player
		FVector ActorsLocal3dVector = Player->GetTransform().InverseTransformPosition(ActorToPlace->GetActorLocation());
		
		//Rotate the vector by 90 degrees counter-clockwise in order to have a valid rotation in our radar
		ActorsLocal3dVector = FRotator(0.f, -90.f, 0.f).RotateVector(ActorsLocal3dVector);
		
		//Apply the given distance scale
		ActorsLocal3dVector /= RadarDistanceScale;

		//Return a 2d vector based on the 3d vector we've created above
		return FVector2D(ActorsLocal3dVector);
	}

	return FVector2D(0,0);
}

void AMinimapHUD::DrawRadar()
{
	FVector2D RadarCenter = GetRadarCenterPosition();

	for (float i = 0; i < 360; i+=DegreeStep)
	{
		//We want to draw a circle in order to represent our radar
		//In order to do so, we calculate the sin and cos of almost every degree
		//It it impossible to calculate each and every possible degree because they are infinite
		//Lower the degree step in case you need a more accurate circle representation

		//We multiply our coordinates by radar size 
		//in order to draw a circle with radius equal to the one we will input through the editor
		float fixedX = FMath::Cos(i) * RadarRadius;
		float fixedY = FMath::Sin(i) * RadarRadius;

		//Actual draw
		DrawLine(RadarCenter.X, RadarCenter.Y, RadarCenter.X + fixedX, RadarCenter.Y + fixedY, FLinearColor::Gray, 1.f);
	}
}

void AMinimapHUD::DrawPlayerInRadar()
{
	FVector2D RadarCenter = GetRadarCenterPosition();

	DrawRect(FLinearColor::Blue, RadarCenter.X, RadarCenter.Y, DrawPixelSize, DrawPixelSize);
}

void AMinimapHUD::PerformRadarRaycast()
{
	APawn* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	if (Player)
	{
		TArray<FHitResult> HitResults;
		FVector EndLocation = Player->GetActorLocation();
		EndLocation.Z += SphereHeight;

		FCollisionShape CollisionShape;
		CollisionShape.ShapeType = ECollisionShape::Sphere;
		CollisionShape.SetSphere(SphereRadius);

		//Perform a the necessary sweep for actors.
		//In case you're wondering how this works, read my raycast tutorial here: http://wp.me/p6hvtS-5F
		GetWorld()->SweepMultiByChannel(HitResults, Player->GetActorLocation(), EndLocation, FQuat::Identity, ECollisionChannel::ECC_WorldDynamic, CollisionShape);

		for (auto It : HitResults)
		{
			AActor* CurrentActor = It.GetActor();
			//In case the actor contains the word "Radar" as a tag, add it to our array
			if (CurrentActor && CurrentActor->ActorHasTag("Radar")) RadarActors.Add(CurrentActor);
		}
	}
}

void AMinimapHUD::DrawRaycastedActors()
{
	FVector2D RadarCenter = GetRadarCenterPosition();

	for (auto It : RadarActors)
	{
		FVector2D convertedLocation = ConvertWorldLocationToLocal(It);

		//We want to clamp the location of our actors in order to make sure
		//that we display them inside our radar

		//To do so, I've created the following temporary vector in order to access
		//the GetClampedToMaxSize2d function. This functions returns a clamped vector (if needed)
		//to match our max length
		FVector tempVector = FVector(convertedLocation.X, convertedLocation.Y, 0.f);

		//Subtract the pixel size in order to make the radar display more accurate
		tempVector = tempVector.GetClampedToMaxSize2D(RadarRadius - DrawPixelSize);

		//Assign the converted X and Y values to the vector we want to display
		convertedLocation.X = tempVector.X;
		convertedLocation.Y = tempVector.Y;

		DrawRect(FLinearColor::Red, RadarCenter.X + convertedLocation.X, RadarCenter.Y + convertedLocation.Y, DrawPixelSize, DrawPixelSize);
	}
}

