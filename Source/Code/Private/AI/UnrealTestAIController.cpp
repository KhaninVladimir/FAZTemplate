// Copyright 2022 Khanin Vladimir (FAZIC)


#include "AI/UnrealTestAIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Perception/AIPerceptionTypes.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISense.h"
#include "Perception/AISense_Damage.h"

#include "Player/UnrealTestPlayerState.h"
#include "Navigation/CrowdFollowingComponent.h"

#include "Character/UnrealTestCharacter.h"

AUnrealTestAIController::AUnrealTestAIController(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))	
{
	bWantsPlayerState = true;
	BlackboardComp = ObjectInitializer.CreateDefaultSubobject<UBlackboardComponent>(this, TEXT("BlackboardComp"));
	AIPerceptionComp = ObjectInitializer.CreateDefaultSubobject<UAIPerceptionComponent>(this, TEXT("AIPerception"));
}

FGenericTeamId AUnrealTestAIController::GetGenericTeamId() const
{
	auto PS = Cast<AUnrealTestPlayerState>(PlayerState);
	if (!PS)
		return 255;

	return PS->GetTeamID();
}


void AUnrealTestAIController::BeginPlay()
{
	Super::BeginPlay();
	if (!HasAuthority())
		return;
	
	if (auto PS = Cast<AUnrealTestPlayerState>(PlayerState))
		PS->SetIsABot(true);

	AIPerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &ThisClass::OnTargetPerceptionUpdatedCallback);
}

void AUnrealTestAIController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}


void AUnrealTestAIController::OnPossess(class APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (!HasAuthority())
		return;
	
	if (PerceptionComponent)
		PerceptionComponent->ForgetAll();

	RunBehaviorTree(BehaviorTree);
}

void AUnrealTestAIController::OnUnPossess()
{
	Super::OnUnPossess();
	if (!HasAuthority())
		return;

	if (PerceptionComponent)
		PerceptionComponent->ForgetAll();
}

void AUnrealTestAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!HasAuthority())
		return;

	SetFocus(nullptr);

	auto CharacterPtr = GetCharacterOwner();
	if (!CharacterPtr)
		return;

	if (!CharacterPtr->IsAlive() || !CharacterPtr->IsPawnControlled())
		return;

	if (!ActiveTargetWeakPtr.IsValid())
		return;

	TimeActiveTarget += DeltaSeconds;
	const auto DistanceSqr = DistanceFaceToTarget * DistanceFaceToTarget;
	if (FaceToTarget || DistanceSqr > (ActiveTargetWeakPtr->GetActorLocation() - CharacterPtr->GetActorLocation()).SizeSquared())
	{
		if (auto TargetInfoPtr = GetTargetPerseptionInfo())
		{
			float OutAge = 0.f;
			const auto LastStimulusLocation = TargetInfoPtr->GetLastStimulusLocation(&OutAge);
			if ((FAISystem::InvalidLocation != LastStimulusLocation) && (OutAge > MaxLastStimulusAgeLostFocus))
			{
				SetFocalPoint(LastStimulusLocation, EAIFocusPriority::Gameplay);
			}
			else
			{
				SetFocus(ActiveTargetWeakPtr.Get(), EAIFocusPriority::Gameplay);
			}
			return;
		}
	}
}

void AUnrealTestAIController::OnTargetPerceptionUpdatedCallback(AActor* UpdatedActor, FAIStimulus Stimulus)
{
	if (auto CharacterPtr = Cast<AUnrealTestCharacter>(UpdatedActor))
	{
		if (!CharacterPtr->IsAlive() || !CharacterPtr->IsPawnControlled())
			return;

		FActorPerceptionBlueprintInfo Perception;
		AIPerceptionComp->GetActorsPerception(CharacterPtr, Perception);

		if (CharacterPtr->GetGenericTeamId() != GetGenericTeamId())
			SetActiveTarget(CharacterPtr);
	}
}

const FActorPerceptionInfo* AUnrealTestAIController::GetTargetPerseptionInfo() const
{
	if (!AIPerceptionComp)
		return nullptr;

	if (!ActiveTargetWeakPtr.IsValid())
		return nullptr;

	return AIPerceptionComp->GetActorInfo(*ActiveTargetWeakPtr.Get());
}

AUnrealTestCharacter* AUnrealTestAIController::GetActiveTarget() const
{
	if (ActiveTargetWeakPtr.IsValid())
		return ActiveTargetWeakPtr.Get();

	return nullptr;
}

void AUnrealTestAIController::SetActiveTarget(AUnrealTestCharacter* InTarget)
{
	if (ActiveTargetWeakPtr.Get() == InTarget)
		return;

	ActiveTargetWeakPtr = InTarget;
	TimeActiveTarget = 0.f;
}

void AUnrealTestAIController::SetTargetLocation(const FVector& NewLocation)
{
	if (BlackboardComp)
		BlackboardComp->SetValueAsVector(TEXT("TargetLocation"), NewLocation);
}

void AUnrealTestAIController::ClearTargetLocation()
{
	if (BlackboardComp)
		BlackboardComp->ClearValue(TEXT("TargetLocation"));
}


AUnrealTestCharacter* AUnrealTestAIController::GetCharacterOwner() const
{
	if (!CharacterWeakPtr.IsValid())
		CharacterWeakPtr = Cast<AUnrealTestCharacter>(GetPawn());
	return CharacterWeakPtr.Get();
}
