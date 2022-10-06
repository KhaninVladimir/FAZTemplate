#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

class UGameplayTagsManager;

/**
 *	Singleton containing native gameplay tags.
 */
struct FUnrealTestGameplayTags
{
public:

	static const FUnrealTestGameplayTags& Get() { return GameplayTags; }

	static void InitializeNativeTags();

	static FGameplayTag FindTagByString(FString TagString, bool bMatchPartialString = false);

public:
	FGameplayTag State_Dead;

	FGameplayTag GameplayEvent_Death;

	FGameplayTag Gameplay_Damage;

	FGameplayTag Skill_First;

protected:

	void AddAllTags(UGameplayTagsManager& Manager);
	void AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment);

private:

	static FUnrealTestGameplayTags GameplayTags;
};
