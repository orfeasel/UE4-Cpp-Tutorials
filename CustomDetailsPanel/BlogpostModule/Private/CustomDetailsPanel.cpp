// Fill out your copyright notice in the Description page of Project Settings.

#include "CustomDetailsPanel.h"
#include "IDetailsView.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "DetailCategoryBuilder.h"
#include "Widgets/SNullWidget.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/SBoxPanel.h"
#include "Text.h"
#include "FancyCube.h"
#include "UObject/Class.h"


TSharedRef<IDetailCustomization> FCustomDetailsPanel::MakeInstance()
{
	return MakeShareable(new FCustomDetailsPanel);
}

void FCustomDetailsPanel::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	//Edits a category. If it doesn't exist it creates a new one
	IDetailCategoryBuilder& CustomCategory = DetailBuilder.EditCategory("CustomCategory");

	//Store the currently selected objects from the viewport to the SelectedObjects array.
	DetailBuilder.GetObjectsBeingCustomized(SelectedObjects);

	//Adding a custom row
	CustomCategory.AddCustomRow(FText::FromString("Outline Color Changing Category"))
		.ValueContent()
		.VAlign(VAlign_Center) // set vertical alignment to center
		.MaxDesiredWidth(250)
		[ //With this operator we declare a new slate object inside our widget row
		  //In this case the slate object is a button
			SNew(SButton)
			.VAlign(VAlign_Center)
		.OnClicked(this, &FCustomDetailsPanel::ClickedOnButton) //Binding the OnClick function we want to execute when this object is clicked
		.Content()
		[ //We create a new slate object inside our button. In this case a text block with the content of "Change Color"
		  //If you ever coded a UMG button with a text on top of it you will notice that the process is quite the same
		  //Meaning, you first declare a button which has various events and properties and then you place a Text Block widget
		  //inside the button's widget to display text
			SNew(STextBlock).Text(FText::FromString("Change Color!"))
		]
		];
	
}

FReply FCustomDetailsPanel::ClickedOnButton()
{	
	if (GEngine)
	{
		for (const TWeakObjectPtr<UObject>& Object : SelectedObjects)
		{
			AFancyCube* FancyCube = Cast<AFancyCube>(Object.Get());
			if (FancyCube)
			{
				FancyCube->AssignColor(FLinearColor::MakeRandomColor());
			}
		}
		GLog->Log("fancy cubes got a nice random color!");
	}
	return FReply::Handled();
}