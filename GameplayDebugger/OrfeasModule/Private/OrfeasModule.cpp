#include "OrfeasModule.h"

DEFINE_LOG_CATEGORY(OrfeasModule);

#if WITH_GAMEPLAY_DEBUGGER
#include "GameplayDebugger.h"
#include "CustomGameplayDebugger.h"
#endif


#define LOCTEXT_NAMESPACE "FOrfeasModule"

void FOrfeasModule::StartupModule()
{
	UE_LOG(OrfeasModule, Warning, TEXT("Orfeas module has started!"));

#if WITH_GAMEPLAY_DEBUGGER

	//If the gameplay debugger is available, register the category and notify the editor about the changes
	IGameplayDebugger& GameplayDebuggerModule = IGameplayDebugger::Get();

	GameplayDebuggerModule.RegisterCategory("OrfeasCustomCategory", IGameplayDebugger::FOnGetCategory::CreateStatic(&FGameplayDebuggerCategory_Orfeas::MakeInstance), EGameplayDebuggerCategoryState::EnabledInGameAndSimulate);

	GameplayDebuggerModule.NotifyCategoriesChanged();

#endif
}

void FOrfeasModule::ShutdownModule()
{
	UE_LOG(OrfeasModule, Warning, TEXT("Orfeas module has shut down"));

#if WITH_GAMEPLAY_DEBUGGER
	//If the gameplay debugger is available, unregister the category
	if (IGameplayDebugger::IsAvailable())
	{
		IGameplayDebugger& GameplayDebuggerModule = IGameplayDebugger::Get();
		GameplayDebuggerModule.UnregisterCategory("OrfeasCustomCategory");
	}
#endif
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FOrfeasModule,OrfeasModule)