// Copyright 2022 Khanin Vladimir (FAZIC)

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"
#include "UnrealTestGameplayAbility.generated.h"

class AUnrealTestCharacter;
class AUnrealTestPlayerController;

UENUM(BlueprintType)
enum class EUTAbilityInputID : uint8
{
	None				UMETA(DisplayName = "None"),
	Confirm				UMETA(DisplayName = "Confirm"),
	Cancel				UMETA(DisplayName = "Cancel"),
	FirstAbility		UMETA(DisplayName = "FirstAbility"),
};

/**
 * The base gameplay ability class.
 */
UCLASS()
class UNREALTEST_API UUnrealTestGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	friend class UUnrealTestAbilitySystemComponent;

public:
	UUnrealTestGameplayAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
	UTexture2D* AbilityIcon;

	// Abilities with this set will automatically activate when the input is pressed
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
	EUTAbilityInputID AbilityInputID = EUTAbilityInputID::None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Ability")
	bool bActivateOnInput;

protected:
	UFUNCTION(BlueprintCallable, Category = "Ability")
	UUnrealTestAbilitySystemComponent* GetUTAbilitySystemComponentFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "Ability")
	AUnrealTestPlayerController* GetUTPlayerControllerFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "Ability")
	AController* GetControllerFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "Ability")
	AUnrealTestCharacter* GetUTCharacterFromActorInfo() const;

private:

	mutable TWeakObjectPtr<AUnrealTestCharacter> CharacterPtr;
	mutable TWeakObjectPtr<AController> ControllerPtr;
	mutable TWeakObjectPtr<UUnrealTestAbilitySystemComponent> ASCPtr;
};
