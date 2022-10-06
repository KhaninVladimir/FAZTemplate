#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "UnrealTestCharacterListData.generated.h"

class AUnrealTestCharacter;

USTRUCT(BlueprintType)
struct FUTCharacterInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FString DisplayName;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (AllowAbstract = "false"))
	TSubclassOf<AUnrealTestCharacter> CharacterClass;
};

/**
 * Data container for debug Character.
 */
UCLASS(BlueprintType)
class UUnrealTestCharacterListData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TArray<FUTCharacterInfo> CharacterList;
};
