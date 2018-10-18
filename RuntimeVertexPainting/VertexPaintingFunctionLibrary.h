// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "VertexPaintingFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class VERTEXPAINTING_API UVertexPaintingFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, Category = VertexPainting)
	static void PaintSMVertices(UStaticMeshComponent* SMComp);
	
	
};
