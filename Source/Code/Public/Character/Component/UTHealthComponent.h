// Copyright 2022 Khanin Vladimir (FAZIC)

#pragma once

#include "CoreMinimal.h"
#include "Character/Component/UTCharacterComponent.h"
#include "GameplayEffectTypes.h"
#include "UTHealthComponent.generated.h"

class UUnrealTestAbilitySystemComponent;
class UUnrealTestAttributeSet;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnDying, float, DamageAmount, AActor*, EventInstigator, AActor*, DamageCauser);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnHealthChanged, UUTHealthComponent*, HealthComponent, float, OldValue, float, NewValue, AActor*, Instigator);

/**
 * Component for Character health
 */
UCLASS()
class UNREALTEST_API UUTHealthComponent : public UUTCharacterComponent
{
	GENERATED_BODY()
public:	
	UUTHealthComponent(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintPure, Category = "Health")
	static UUTHealthComponent* FindHealthComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UUTHealthComponent>() : nullptr); }

	virtual void InitializeWithAbilitySystem(UUnrealTestAbilitySystemComponent* InASC) override;
	virtual void UninitializeFromAbilitySystem() override;

	UFUNCTION(BlueprintCallable, Category = "Health")
	float GetMaxHealth() const;
	UFUNCTION(BlueprintCallable, Category = "Health")
	float GetHealth() const;
	// Returns the current health in the range [0.0, 1.0].
	UFUNCTION(BlueprintCallable, Category = "Health")
	float GetHealthNormalized() const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Lyra|Health", Meta = (ExpandBoolAsExecs = "ReturnValue"))
	bool IsAlive() const { return !bDead; }

	// Applies enough damage to kill the owner.
	virtual void DamageSelfDestruct(bool bFellOutOfWorld = false);

	virtual void OnDeath();

	UPROPERTY(BlueprintAssignable)
	FOnHealthChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FOnHealthChanged OnMaxHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FOnDying OnDying;

protected:

	virtual void BeginPlay() override;

	void ClearGameplayTags();

	virtual void HandleHealthChanged(const FOnAttributeChangeData& ChangeData);
	virtual void HandleMaxHealthChanged(const FOnAttributeChangeData& ChangeData);
	virtual void HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec& DamageEffectSpec, float DamageMagnitude);

	UFUNCTION()
	virtual void OnRep_DeathState(bool OldDeathState);

	// Replicated state used to handle dying.
	UPROPERTY(ReplicatedUsing = OnRep_DeathState)
	bool bDead;	
};
