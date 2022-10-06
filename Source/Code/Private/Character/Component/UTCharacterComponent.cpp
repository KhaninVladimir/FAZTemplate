// Copyright 2022 Khanin Vladimir (FAZIC)


#include "Character/Component/UTCharacterComponent.h"
#include "Character/Component/UTCharacterMovementComponent.h"
#include "Character/UnrealTestCharacter.h"
#include "AbilitySystem/UnrealTestAbilitySystemComponent.h"
#include "AbilitySystem/Attribute/UnrealTestAttributeSet.h"
#include "AbilitySystemGlobals.h"
#include "UnrealTestLog.h"


// Sets default values for this component's properties
UUTCharacterComponent::UUTCharacterComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
}

void UUTCharacterComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UUTCharacterComponent::OnUnregister()
{
	UninitializeFromAbilitySystem();

	Super::OnUnregister();
}

void UUTCharacterComponent::InitializeWithAbilitySystem(UUnrealTestAbilitySystemComponent* InASC)
{
	AActor* Owner = GetOwner();
	if (!Owner)
		return;

	if (!InASC)
	{
		UE_LOG(UTComponentLog, Error, TEXT("UUTCharacterComponent: Cannot initialize health component for owner [%s] with NULL ability system."), *GetNameSafe(Owner));
		return;
	}

	AbilitySystemComponent = InASC;

	AttributeSet = AbilitySystemComponent->GetSet<UUnrealTestAttributeSet>();
	if (!AttributeSet)
	{
		UE_LOG(UTComponentLog, Error, TEXT("UUTCharacterComponent: Cannot initialize health component for owner [%s] with NULL health set on the ability system."), *GetNameSafe(Owner));
		return;
	}
}

void UUTCharacterComponent::UninitializeFromAbilitySystem()
{
	AttributeSet = nullptr;
	AbilitySystemComponent = nullptr;
}

UUnrealTestAbilitySystemComponent* UUTCharacterComponent::GetAbilitySystemComponent() const
{
	if (!AbilitySystemComponent.IsValid() && GetOwner())
		AbilitySystemComponent = Cast<UUnrealTestAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner()));

	return AbilitySystemComponent.Get();
}

const UUnrealTestAttributeSet* UUTCharacterComponent::GetAttributeSet() const
{
	if (!IsValid(AttributeSet) && GetAbilitySystemComponent())
		AttributeSet = GetAbilitySystemComponent()->GetSet<UUnrealTestAttributeSet>();

	return AttributeSet;
}

UUTCharacterMovementComponent* UUTCharacterComponent::GetMovementComponent() const
{
	if (!MovementComponentWeakPtr.IsValid())
	{
		if (auto CharacterPtr = GetCharacterOwner())
			MovementComponentWeakPtr = Cast<UUTCharacterMovementComponent>(CharacterPtr->GetMovementComponent());
	}
	return MovementComponentWeakPtr.Get();
}

AUnrealTestCharacter* UUTCharacterComponent::GetCharacterOwner() const
{
	if (!CharacterOwner.IsValid())
		CharacterOwner = Cast<AUnrealTestCharacter>(GetOwner());
	return CharacterOwner.Get();
}
