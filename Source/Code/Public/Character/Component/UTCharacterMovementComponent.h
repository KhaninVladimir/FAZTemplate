// Copyright 2022 Khanin Vladimir (FAZIC)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UTCharacterMovementComponent.generated.h"

class UUnrealTestAttributeSet;
class UUnrealTestAbilitySystemComponent;
class AUnrealTestCharacter;
/**
 * 
 */
UCLASS()
class UNREALTEST_API UUTCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
public:
	virtual float GetMaxSpeed() const override;

protected:

	const UUnrealTestAttributeSet* GetAttributeSet() const;
	UUnrealTestAbilitySystemComponent* GetAbilitySystemComponent() const;
	AUnrealTestCharacter* GetCharacter() const;

private:
	mutable const UUnrealTestAttributeSet* AttributeSet;
	mutable TWeakObjectPtr<UUnrealTestAbilitySystemComponent> AbilitySystemComponent;
	mutable TWeakObjectPtr<AUnrealTestCharacter> OwnerCharacterWeakPtr;
};
