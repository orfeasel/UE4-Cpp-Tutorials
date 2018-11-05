#include "BlogpostModule.h"
#include "FancyCube.h"
#include "CustomDetailsPanel.h"
#include "PropertyEditorModule.h"

DEFINE_LOG_CATEGORY(BlogpostModule);

#define LOCTEXT_NAMESPACE "FBlogpostModule"

void FBlogpostModule::StartupModule()
{
	UE_LOG(BlogpostModule, Warning, TEXT("BlogpostModule module has started!"));
	//Get the property module
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	//Register the custom details panel we have created
	PropertyModule.RegisterCustomClassLayout(AFancyCube::StaticClass()->GetFName(), FOnGetDetailCustomizationInstance::CreateStatic(&FCustomDetailsPanel::MakeInstance));
}

void FBlogpostModule::ShutdownModule()
{
	UE_LOG(BlogpostModule, Warning, TEXT("BlogpostModule module has shut down"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FBlogpostModule,BlogpostModule)