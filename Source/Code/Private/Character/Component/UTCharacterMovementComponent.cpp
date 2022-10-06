// Copyright 2022 Khanin Vladimir (FAZIC)


#include "Character/Component/UTCharacterMovementComponent.h"
#include "Character/UnrealTestCharacter.h"
#include "AbilitySystem/UnrealTestAbilitySystemComponent.h"
#include "AbilitySystem/Attribute/UnrealTestAttributeSet.h"

float UUTCharacterMovementComponent::GetMaxSpeed() const
{
	if (auto attribute = GetAttributeSet())
		return attribute->GetMoveSpeed();

	return Super::GetMaxSpeed();
}

const UUnrealTestAttributeSet* UUTCharacterMovementComponent::GetAttributeSet() const
{
	if (!IsValid(AttributeSet) && GetAbilitySystemComponent())
		AttributeSet = GetAbilitySystemComponent()->GetSet<UUnrealTestAttributeSet>();

	return AttributeSet;
}

UUnrealTestAbilitySystemComponent* UUTCharacterMovementComponent::GetAbilitySystemComponent() const
{
	if (!AbilitySystemComponent.IsValid() && GetCharacter())
		AbilitySystemComponent = GetCharacter()->GetUTAbilitySystemComponent();

	return AbilitySystemComponent.Get();
}

AUnrealTestCharacter* UUTCharacterMovementComponent::GetCharacter() const
{
	if (!OwnerCharacterWeakPtr.IsValid())
		OwnerCharacterWeakPtr = Cast<AUnrealTestCharacter>(PawnOwner);

	return 	OwnerCharacterWeakPtr.Get();
}