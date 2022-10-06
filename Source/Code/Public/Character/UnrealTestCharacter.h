// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "Runtime/AIModule/Classes/GenericTeamAgentInterface.h"
#include "UnrealTestCharacter.generated.h"

class UUTCharacterMovementComponent;
class UUnrealTestAbilitySystemComponent;
class UUnrealTestAttributeSet;

class AUnrealTestGameState;
class AUnrealTestGameMode;
class AUnrealTestPlayerState;

/*
* Base class for character
*/
UCLASS(config=Game)
class AUnrealTestCharacter : public ACharacter, public IAbilitySystemInterface, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lyra|Character", Meta = (AllowPrivateAccess = "true"))
	class UUTHealthComponent* HealthComponent;
public:
	AUnrealTestCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void DetachFromControllerPendingDestroy() override;

	virtual bool IsAlive() const;

	UFUNCTION(BlueprintCallable)
	FString GetCharacterName() const;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Input)
	float TurnRateGamepad;

	//List who assisted in kill of this character
	TArray<AUnrealTestPlayerState*> AssistsList;

	virtual void OnDeath();	

	virtual FGenericTeamId GetGenericTeamId() const override;

protected:
	// Default abilities for this Character. These will be removed on Character death and regiven if Character respawns.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<class UUnrealTestGameplayAbility>> CharacterAbilities;

	// Default attributes for a character for initializing on spawn/respawn.
	// This is an instant GE that overrides the values for attributes that get reset on spawn/respawn.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	TSubclassOf<class UGameplayEffect> DefaultAttributes;

	// Grant abilities on the Server. The Ability Specs will be replicated to the owning client.
	virtual void AddCharacterAbilities();
	// Removes all CharacterAbilities. Can only be called by the Server.
	virtual void RemoveCharacterAbilities();
	virtual void InitializeAttributes();

	//Called on server
	UFUNCTION()
	virtual void CharacterDied(float DamageAmount, AActor* EventInstigator, AActor* DamageCauser);
	virtual void Die(AController* Killer = nullptr, AActor* DamageCauser = nullptr, bool bSimple = false);
	
	virtual void DisableMovementAndCollision();
	virtual void ActivateRagdollPhysics();
	virtual void DestroyDueToDeath();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	UFUNCTION(BlueprintImplementableEvent, Meta = (DisplayName = "OnRep_PlayerState"))
	void K2_OnRep_PlayerState();

	void BindASCInput();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	void DisableCotrollerRotation();
	void ConfigureCharacterMovement(class UCharacterMovementComponent* characterMovement);
	void SetCameraBoom();
	void SetFollowCamera();

	void JumpBinding(class UInputComponent* PlayerInputComponent);
	void MovementBinding(class UInputComponent* PlayerInputComponent);
	void TurnBinding(class UInputComponent* PlayerInputComponent);
	void LookUpBinding(class UInputComponent* PlayerInputComponent);
	void TouchBinding(class UInputComponent* PlayerInputComponent);

	const float TURN_RATE_GAMEPAD = 50.f;
	const float JUMP_Z_VELOCITY= 700.f;
	const float AIR_CONTROL = 0.35f;
	const float MAX_WALK_SPEED = 500.f;
	const float MIN_ANALOG_WALK_SPEED = 20.f;
	const float BRAKING_DECELERATION_WALKING = 2000.f;

public:
	// Implement IAbilitySystemInterface
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UUnrealTestAbilitySystemComponent* GetUTAbilitySystemComponent() const;
	const UUnrealTestAttributeSet* GetAttributeSet() const;

protected:

	UPROPERTY()
	mutable TWeakObjectPtr<UAbilitySystemComponent> AbilitySystemComponentPtr;

	UFUNCTION(BlueprintCallable)
	UUTCharacterMovementComponent* GetCharacterMovementComponent() const;

	UFUNCTION(BlueprintCallable)
	AUnrealTestGameState* GetUTGameState() const;

	UFUNCTION(BlueprintCallable)
	AUnrealTestGameMode* GetUTGameMode() const;

	UFUNCTION(BlueprintCallable)
	AUnrealTestPlayerState* GetUTPlayerState() const;

private:

	bool bASCInputBound;

	mutable const UUnrealTestAttributeSet* AttributeSetPtr;
	mutable TWeakObjectPtr<UUTCharacterMovementComponent> CharacterMovementPtr;
	mutable TWeakObjectPtr<AUnrealTestGameState> GameStatePtr;
	mutable TWeakObjectPtr<AUnrealTestGameMode> GameModePtr;
	mutable TWeakObjectPtr<AUnrealTestPlayerState> PlayerStatePtr;
};

