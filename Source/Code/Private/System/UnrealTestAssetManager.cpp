// Copyright 2022 Khanin Vladimir (FAZIC)


#include "System/UnrealTestAssetManager.h"
#include "System/UnrealTestGameplayTags.h"

DEFINE_LOG_CATEGORY_STATIC(UTAssetManagerLog, Log, All);

UUnrealTestAssetManager::UUnrealTestAssetManager()
{

}

UUnrealTestAssetManager& UUnrealTestAssetManager::Get()
{
	check(GEngine);

	if (UUnrealTestAssetManager* Singleton = Cast<UUnrealTestAssetManager>(GEngine->AssetManager))
	{
		return *Singleton;
	}

	UE_LOG(UTAssetManagerLog, Fatal, TEXT("Invalid AssetManagerClassName in DefaultEngine.ini.  It must be set to UUnrealTestAssetManager!"));

	// Fatal error above prevents this from being called.
	return *NewObject<UUnrealTestAssetManager>();
}

void UUnrealTestAssetManager::StartInitialLoading()
{
	SCOPED_BOOT_TIMING("UUnrealTestAssetManager::StartInitialLoading");

	// This does all of the scanning, need to do this now even if loads are deferred
	Super::StartInitialLoading();

	InitializeTagsSystem();
}

void UUnrealTestAssetManager::InitializeTagsSystem()
{
	SCOPED_BOOT_TIMING("UUnrealTestAssetManager::InitializeTagsSystem");

	FUnrealTestGameplayTags::InitializeNativeTags();
}