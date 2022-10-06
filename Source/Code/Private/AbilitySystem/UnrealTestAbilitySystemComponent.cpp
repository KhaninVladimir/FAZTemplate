// Copyright 2022 Khanin Vladimir (FAZIC)


#include "AbilitySystem/UnrealTestAbilitySystemComponent.h"
#include "AbilitySystem/UnrealTestGameplayAbility.h"
#include "GameplayEffectTypes.h"
#include "NativeGameplayTags.h"
#include "UnrealTestLog.h"

UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_AbilityInputBlocked, "Gameplay.AbilityInputBlocked");

UUnrealTestAbilitySystemComponent::UUnrealTestAbilitySystemComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UUnrealTestAbilitySystemComponent::AbilityLocalInputPressed(int32 InputID)
{
	// Consume the input if this InputID is overloaded with GenericConfirm/Cancel and the GenericConfim/Cancel callback is bound
	if (IsGenericConfirmInputBound(InputID))
	{
		LocalInputConfirm();
		return;
	}

	if (IsGenericCancelInputBound(InputID))
	{
		LocalInputCancel();
		return;
	}

	// ---------------------------------------------------------

	ABILITYLIST_SCOPE_LOCK();
	for (FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		if (Spec.InputID == InputID)
		{
			if (Spec.Ability)
			{
				Spec.InputPressed = true;
				if (Spec.IsActive())
				{
					if (Spec.Ability->bReplicateInputDirectly && IsOwnerActorAuthoritative() == false)
					{
						ServerSetInputPressed(Spec.Handle);
					}

					AbilitySpecInputPressed(Spec);

					// Invoke the InputPressed event. This is not replicated here. If someone is listening, they may replicate the InputPressed event to the server.
					InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
				}
				else
				{
					UUnrealTestGameplayAbility* GA = Cast<UUnrealTestGameplayAbility>(Spec.Ability);
					if (GA && GA->bActivateOnInput)
					{
						// Ability is not active, so try to activate it
						TryActivateAbility(Spec.Handle);
					}
				}
			}
		}
	}
}

UUnrealTestGameplayAbility* UUnrealTestAbilitySystemComponent::GetAbilityByInputID(EUTAbilityInputID InputID) const
{
	for (auto AbilitySpec : GetActivatableAbilities())
	{
		if (auto ability = Cast<UUnrealTestGameplayAbility>(AbilitySpec.Ability))
		{
			if (ability->AbilityInputID == InputID)
			{
				return ability;
			}
		}
	}
	return nullptr;
}

void UUnrealTestAbilitySystemComponent::OnGiveAbility(FGameplayAbilitySpec& AbilitySpec)
{
	Super::OnGiveAbility(AbilitySpec);

	OnAbilitiesChanged.Broadcast();
}

void UUnrealTestAbilitySystemComponent::OnRemoveAbility(FGameplayAbilitySpec& AbilitySpec)
{
	Super::OnRemoveAbility(AbilitySpec);

	OnAbilitiesChanged.Broadcast();
}