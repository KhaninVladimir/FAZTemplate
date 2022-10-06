// Copyright 2022 Khanin Vladimir (FAZIC)

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Runtime/AIModule/Classes/Perception/AIPerceptionComponent.h"
#include "UnrealTestAIController.generated.h"

class AUnrealTestCharacter;
class UAIPerceptionComponent;

/**
 * Base class for AI Controller
 */
UCLASS()
class UNREALTEST_API AUnrealTestAIController : public AAIController
{
	GENERATED_BODY()
public:

	UFUNCTION()
	void OnTargetPerceptionUpdatedCallback(AActor* UpdatedActor, FAIStimulus Stimulus);

	AUnrealTestCharacter* GetActiveTarget() const;
	void SetActiveTarget(AUnrealTestCharacter* InTarget);
	float GetTimeActiveTarget() const { return TimeActiveTarget; }

	void SetTargetLocation(const FVector& NewLocation);
	void ClearTargetLocation();

	UAIPerceptionComponent* GetPerceptionCmp() const { return  AIPerceptionComp; }

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	class UBehaviorTree* BehaviorTree;

	AUnrealTestCharacter* GetCharacterOwner() const;

protected:
	AUnrealTestAIController(const class FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaSeconds) override;

	/* Called whenever the controller possesses a character bot */
	virtual void OnPossess(class APawn* InPawn) override;
	virtual void OnUnPossess() override;

	virtual FGenericTeamId GetGenericTeamId() const override;

	UPROPERTY()
	class UBlackboardComponent* BlackboardComp = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UAIPerceptionComponent* AIPerceptionComp = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float MaxLastStimulusAgeLostFocus = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	bool bAlwaysFocusToTarget = false;

	TWeakObjectPtr<AUnrealTestCharacter> ActiveTargetWeakPtr;
	
	const FActorPerceptionInfo* GetTargetPerseptionInfo() const;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float DistanceFaceToTarget = 2000.f;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	bool FaceToTarget = true;

private:
	mutable TWeakObjectPtr<AUnrealTestCharacter> CharacterWeakPtr;
	float TimeActiveTarget = 0.f;
};
