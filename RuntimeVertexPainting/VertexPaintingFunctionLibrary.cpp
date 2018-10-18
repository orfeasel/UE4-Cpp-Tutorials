// Fill out your copyright notice in the Description page of Project Settings.

#include "VertexPaintingFunctionLibrary.h"

void UVertexPaintingFunctionLibrary::PaintSMVertices(UStaticMeshComponent* SMComp)
{
	if (!SMComp) return;

	//Get the static mesh that we're going to paint
	UStaticMesh* SM = SMComp->GetStaticMesh();
	if (SM)
	{
		//Get the vertex buffer from the 1st lod
		//FPositionVertexBuffer* PositionVertexBuffer = &SM->RenderData->LODResources[0].VertexBuffers.PositionVertexBuffer;
		
		//Make sure that we have at least 1 LOD
		SMComp->SetLODDataCount(1, SMComp->LODData.Num());
		FStaticMeshComponentLODInfo* LODInfo = &SMComp->LODData[0]; //We're going to modify the 1st LOD only

		//Empty the painted vertices and assign a new color vertex buffer which will contain the new colors for each vertex
		LODInfo->PaintedVertices.Empty();
		LODInfo->OverrideVertexColors = new FColorVertexBuffer();

		//We're going to use the LODResources to get the total number of vertices that the provided mesh has
		FStaticMeshLODResources& LodResources = SM->RenderData->LODResources[0];
		
		//Creating a color array
		TArray<FColor> RandomColorArray;
		//Since we know beforehand the number of elements we might as well reserve the memory now
		RandomColorArray.Reserve(LodResources.GetNumVertices() - 1);
		
		for (int32 i = 0; i < LodResources.GetNumVertices(); i++)
		{
			//Generate a random color for the current vertex
			RandomColorArray.Add(FColor::MakeRandomColor());
		}

		//Initialize the new vertex colros with the array we created above
		LODInfo->OverrideVertexColors->InitFromColorArray(RandomColorArray);

		//Initialize resource and mark render state of object as dirty in order for the engine to re-render it
		BeginInitResource(LODInfo->OverrideVertexColors);
		SMComp->MarkRenderStateDirty();
	}
}

