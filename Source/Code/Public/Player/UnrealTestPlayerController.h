// Copyright 2022 Khanin Vladimir (FAZIC)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Common/UnrealTestType.h"
#include "Player/UnrealTestCharacterListData.h"
#include "UnrealTestPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOpenUIMenu, bool, bOpen);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerStateChangedDelegate, AUnrealTestPlayerState*, NewPlayerState);

class AUnrealTestPlayerState;
class AUnrealTestCharacter;

/**
 * 
 */
UCLASS()
class UNREALTEST_API AUnrealTestPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerStatisticChanged, FPlayerStatistic, Statistic, class AUnrealTestPlayerState*, PS);
	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FOnPlayerStatisticChanged Client_OnPlayerStatisticChanges;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FClient_OnPlayerKilled, FUTKilledInfo, KilledInfo);
	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FClient_OnPlayerKilled Client_OnPlayerKilled;

	UPROPERTY(BlueprintAssignable)
	FOpenUIMenu OnOpenScoreMenu;

	UPROPERTY(BlueprintAssignable)
	FPlayerStateChangedDelegate OnPlayerStateChangedClient;

	AUnrealTestPlayerController();

	virtual void SetupInputComponent() override;

	UFUNCTION(client, reliable)
	void Client_SendKilledInfo(const FUTKilledInfo& Info);

public:
	void SelectCharacter(int32 Index);
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SelectCharacter(int32 Index);

protected:
	virtual void OnPossess(APawn* aPawn) override;
	virtual void OnUnPossess() override;
	virtual void OnRep_PlayerState() override;
	AUnrealTestPlayerState* GetUTPlayerState() const;

private:
	//UI
	UFUNCTION()
	void OpenScoreMenu();

	UFUNCTION()
	void CloseScoreMenu();

	mutable TWeakObjectPtr<AUnrealTestPlayerState> PlayerStatePtr;


};
