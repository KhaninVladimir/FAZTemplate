// Copyright 2022 Khanin Vladimir (FAZIC)

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "NativeGameplayTags.h"
#include "AbilitySystem/UnrealTestGameplayAbility.h"
#include "UnrealTestAbilitySystemComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAbilitiesChangedDelegate);

/*
*	AbilitySystemComponent for this project
*/
UCLASS()
class UNREALTEST_API UUnrealTestAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
public:
	UUnrealTestAbilitySystemComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// Input bound to an ability is pressed
	virtual void AbilityLocalInputPressed(int32 InputID) override;

	bool CharacterAbilitiesGiven = false;
	bool StartupEffectsApplied = false;

	UFUNCTION(BlueprintCallable)
	UUnrealTestGameplayAbility* GetAbilityByInputID(EUTAbilityInputID InputID) const;

	/** A generic callback anytime abilities are added or removed. */
	UPROPERTY(BlueprintAssignable)
	FOnAbilitiesChangedDelegate OnAbilitiesChanged;

protected:
	virtual void OnGiveAbility(FGameplayAbilitySpec& AbilitySpec) override;
	virtual void OnRemoveAbility(FGameplayAbilitySpec& AbilitySpec) override;
};
