using UnrealBuildTool;

public class OrfeasModule : ModuleRules
{
    public OrfeasModule(TargetInfo Target)
    {
        PublicIncludePaths.AddRange(new string[] { "OrfeasModule/Public" });

        PrivateIncludePaths.AddRange(new string[] { "OrfeasModule/Private" });

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "GDBlogPost" });
        
        if (UEBuildConfiguration.bBuildDeveloperTools || (Target.Configuration != UnrealTargetConfiguration.Shipping && Target.Configuration != UnrealTargetConfiguration.Test))
        {
            PrivateDependencyModuleNames.Add("GameplayDebugger");
            Definitions.Add("WITH_GAMEPLAY_DEBUGGER=1");
        }
        else
        {
            Definitions.Add("WITH_GAMEPLAY_DEBUGGER=0");
        }
    }
}