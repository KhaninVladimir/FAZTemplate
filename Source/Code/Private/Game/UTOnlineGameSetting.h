#pragma once

#include "OnlineSessionSettings.h"

/**
 * General session settings for a Shooter game
 */
class FUTOnlineSessionSettings : public FOnlineSessionSettings
{
public:

	FUTOnlineSessionSettings(bool bIsLAN = false, bool bIsPresence = false, int32 MaxNumPlayers = 4);
	virtual ~FUTOnlineSessionSettings() {}
};

/**
 * General search setting for a Shooter game
 */
class FUTOnlineSearchSettings : public FOnlineSessionSearch
{
public:
	FUTOnlineSearchSettings(bool bSearchingLAN = false, bool bSearchingPresence = false);

	virtual ~FUTOnlineSearchSettings() {}
};

/**
 * Search settings for an empty dedicated server to host a match
 */
class FUTOnlineSearchSettingsEmptyDedicated : public FUTOnlineSearchSettings
{
public:
	FUTOnlineSearchSettingsEmptyDedicated(bool bSearchingLAN = false, bool bSearchingPresence = false);

	virtual ~FUTOnlineSearchSettingsEmptyDedicated() {}
};
