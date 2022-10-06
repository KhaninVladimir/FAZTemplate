
#pragma once
#include "EngineMinimal.h"
#include "UnrealTestType.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMatchStateChanged, FName, StateName);

UENUM(Blueprintable, BlueprintType)
enum class EPlayerPhase : uint8
{
	PP_Preparation,			//Player comming to game
	PP_Battle,				//Player Play in game
	PP_Dead,				//Player was killed
	PP_DeadAndWaitRespawn,	//Player try to respawn
};

USTRUCT(Blueprintable, BlueprintType)
struct FPlayerStatistic
{
	GENERATED_USTRUCT_BODY();

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (ClampMin = "0"))
	FString PlayerName = "Incognito";

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (ClampMin = "0"))
	int32 KillsCount = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (ClampMin = "0"))
	int32 DeathCount = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (ClampMin = "0"))
	int32 AssistCount = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (ClampMin = "0"))
	int32 ScoreCount = 0;

	FPlayerStatistic() = default;
};

USTRUCT(Blueprintable, BlueprintType)
struct FUTKillerPlayerInfo
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString PlayerName = "";

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float PlayerHP = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 KillCount = 0;

	FUTKillerPlayerInfo() = default;
};

USTRUCT(Blueprintable, BlueprintType)
struct FUTKilledInfo
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DamageCount = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FUTKillerPlayerInfo VictimPlayer = FUTKillerPlayerInfo();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FUTKillerPlayerInfo KillerPlayer = FUTKillerPlayerInfo();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class AActor* DamageCauser = nullptr;

	FUTKilledInfo() = default;
};