// Copyright 2022 Khanin Vladimir (FAZIC)


#include "Game/UnrealTestGameSession.h"
#include "Game/UTOnlineGameSetting.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"

namespace
{
	const FString CustomMatchKeyword("Custom");
}

AUnrealTestGameSession::AUnrealTestGameSession(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	OnCreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete);

	MaxPlayers = 8;
}

// Create server session
void AUnrealTestGameSession::OnCreateSessionComplete(FName InSessionName, bool bWasSuccessful)
{
	UE_LOG(LogOnlineGame, Verbose, TEXT("OnCreateSessionComplete %s bSuccess: %d"), *InSessionName.ToString(), bWasSuccessful);

	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();
	if (!OnlineSub)
		return;

	IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
	if (!Sessions.IsValid())
		return;

	Sessions->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);

	OnCreatePresenceSessionComplete().Broadcast(InSessionName, bWasSuccessful);
}

void AUnrealTestGameSession::RegisterServer()
{
	UE_LOG(LogOnlineGame, Log, TEXT("start RegisterServer"));

	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();
	if (!OnlineSub)
		return;

	IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
	if (!Sessions.IsValid())
		return;

	TSharedPtr<class FUTOnlineSessionSettings> UTHostSettings = MakeShareable(new FUTOnlineSessionSettings(false, false, 16));
	UTHostSettings->Set(SETTING_MATCHING_HOPPER, FString("TeamDeathmatch"), EOnlineDataAdvertisementType::DontAdvertise);
	UTHostSettings->Set(SETTING_MATCHING_TIMEOUT, 120.0f, EOnlineDataAdvertisementType::ViaOnlineService);
	UTHostSettings->Set(SETTING_SESSION_TEMPLATE_NAME, FString("GameSession"), EOnlineDataAdvertisementType::DontAdvertise);
	UTHostSettings->Set(SETTING_GAMEMODE, FString("TeamDeathmatch"), EOnlineDataAdvertisementType::ViaOnlineService);
	UTHostSettings->Set(SETTING_MAPNAME, GetWorld()->GetMapName(), EOnlineDataAdvertisementType::ViaOnlineService);
	UTHostSettings->bAllowInvites = true;
	UTHostSettings->bIsDedicated = true;
	UTHostSettings->bIsLANMatch = true;

	HostSettings = UTHostSettings;
	OnCreateSessionCompleteDelegateHandle = Sessions->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegate);
	Sessions->CreateSession(0, NAME_GameSession, *HostSettings);
}
