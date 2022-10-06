// Copyright 2022 Khanin Vladimir (FAZIC)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameSession.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "UnrealTestGameSession.generated.h"

struct FUTGameSessionParams
{
	/** Name of session settings are stored with */
	FName SessionName = NAME_None;
	/** LAN Match */
	bool bIsLAN = false;
	/** Presence enabled session */
	bool bIsPresence = false;
	/** Id of player initiating lobby */
	TSharedPtr<const FUniqueNetId> UserId;
	/** Current search result choice to join */
	int32 BestSessionIdx = 0;

	FUTGameSessionParams() = default;
};

/**
 * 
 */
UCLASS()
class UNREALTEST_API AUnrealTestGameSession : public AGameSession
{
	GENERATED_BODY()
protected:
	AUnrealTestGameSession(const FObjectInitializer& ObjectInitializer);

	//Event triggered when a presence session is created
	DECLARE_EVENT_TwoParams(AUnrealTestGameSession, FOnCreatePresenceSessionComplete, FName /*SessionName*/, bool /*bWasSuccessful*/);
	FOnCreatePresenceSessionComplete CreatePresenceSessionCompleteEvent;

	/** Delegate for creating a new session */
	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
	FOnStartSessionCompleteDelegate OnStartSessionCompleteDelegate;

	FDelegateHandle OnCreateSessionCompleteDelegateHandle;
	FDelegateHandle OnStartSessionCompleteDelegateHandle;

	//return the delegate fired when creating a presence session
	FOnCreatePresenceSessionComplete& OnCreatePresenceSessionComplete() { return CreatePresenceSessionCompleteEvent; }

	//Function fired when a session create request has completed
	virtual void OnCreateSessionComplete(FName InSessionName, bool bWasSuccessful);

	//Called when this instance is starting up as a dedicated server
	virtual void RegisterServer() override;

	FUTGameSessionParams CurrentSessionParams;

	//Current host settings
	TSharedPtr<class FUTOnlineSessionSettings> HostSettings;
	
};
