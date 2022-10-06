// Copyright 2022 Khanin Vladimir (FAZIC)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffect.h"
#include "UTFireActor.generated.h"

UCLASS()
class UNREALTEST_API AUTFireActor : public AActor
{
	GENERATED_BODY()
	
public:
	AUTFireActor();

	UPROPERTY(BlueprintReadWrite, Meta = (ExposeOnSpawn = true))
	float Range;

	UPROPERTY(BlueprintReadWrite, Meta = (ExposeOnSpawn = true))
	FGameplayEffectSpecHandle DamageEffectSpecHandle;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	class UProjectileMovementComponent* ProjectileMovement;

protected:
	virtual void BeginPlay() override;

};
