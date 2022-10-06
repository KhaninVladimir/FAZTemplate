// Copyright 2022 Khanin Vladimir (FAZIC)

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UTCharacterComponent.generated.h"

class AUnrealTestCharacter;
class UUTCharacterMovementComponent;
class UUnrealTestAbilitySystemComponent;
class UUnrealTestAttributeSet;

/**
 * Base class for all components which will be added to Character 
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UNREALTEST_API UUTCharacterComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UUTCharacterComponent(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void OnUnregister() override;
	virtual void InitializeWithAbilitySystem(UUnrealTestAbilitySystemComponent* InASC);
	virtual void UninitializeFromAbilitySystem();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	UUnrealTestAbilitySystemComponent* GetAbilitySystemComponent() const;

	const UUnrealTestAttributeSet* GetAttributeSet() const;

	UFUNCTION(BlueprintCallable)
	AUnrealTestCharacter* GetCharacterOwner() const;

	UFUNCTION(BlueprintCallable)
	UUTCharacterMovementComponent* GetMovementComponent() const;

private:
	mutable const UUnrealTestAttributeSet* AttributeSet;
	mutable TWeakObjectPtr<UUnrealTestAbilitySystemComponent> AbilitySystemComponent;
	mutable TWeakObjectPtr<AUnrealTestCharacter> CharacterOwner;
	mutable TWeakObjectPtr<UUTCharacterMovementComponent> MovementComponentWeakPtr;
		
};
