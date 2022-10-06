// Copyright 2022 Khanin Vladimir (FAZIC)

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "UnrealTestDamageExecution.generated.h"

/**
 * 
 */
UCLASS()
class UNREALTEST_API UUnrealTestDamageExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
public:

	UUnrealTestDamageExecution();

protected:

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
