// Copyright 2022 Khanin Vladimir (FAZIC)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "GenericTeamAgentInterface.h"
#include "UnrealTestPlayerStart.generated.h"

/**
 * 
 */
UCLASS()
class UNREALTEST_API AUnrealTestPlayerStart : public APlayerStart
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	bool IsReady();
	bool IsTeamPlayerStart(FGenericTeamId Id) const;
	void OccupyPoint();

	// Shows how long in second this Point will be inactive after the player spawn on it
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn param")
	float SpawnOccupyDuration = 10.f;

	// If the Vehicle is at this distance from the point, then the point is considered occupied
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn param", meta = (ClampMin = "200", UIMin = "200", UIMax = "1000"))
	float OccupyRadius = 250.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn param")
	FGenericTeamId TeamId;

private:

	UPROPERTY(BlueprintReadOnly, Category = "SpawnPoint", meta = (AllowPrivateAccess = true))
	bool bIsOccupied = false;

	FTimerHandle OccupiedTimerHandle;

	UFUNCTION()
	void FreePoint();
};
