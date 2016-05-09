// Fill out your copyright notice in the Description page of Project Settings.

#include "DialogSystem.h"
#include "DialogUI.h"



void UDialogUI::UpdateSubtitles(TArray<FSubtitle> Subtitles)
{
	if (!Subtitles.IsValidIndex(0)) return;

	//Start a background task in a low priority thread
	(new FAutoDeleteAsyncTask<UMGAsyncTask>(Subtitles, this))->StartBackgroundTask();
}
