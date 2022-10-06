// Copyright 2022 Khanin Vladimir (FAZIC)


#include "Game/UnrealTestPlayerStart.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/CollisionProfile.h"

bool AUnrealTestPlayerStart::IsReady()
{
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Vehicle));

	TArray<AActor*> IgnoreActors;

	TArray<UPrimitiveComponent*> Components;

	bool bHasOverlap = UKismetSystemLibrary::SphereOverlapComponents(this,
		GetActorLocation(),
		OccupyRadius,
		ObjectTypes,
		UPrimitiveComponent::StaticClass(),
		IgnoreActors,
		Components);

	if (bHasOverlap)
	{
		return false;
	}

	return !bIsOccupied;
}

bool AUnrealTestPlayerStart::IsTeamPlayerStart(FGenericTeamId Id) const
{
	return TeamId == FGenericTeamId::NoTeam || TeamId == Id;
}

void AUnrealTestPlayerStart::OccupyPoint()
{
	if (bIsOccupied)
		return;

	bIsOccupied = true;

	GetWorldTimerManager().SetTimer(OccupiedTimerHandle,
		this,
		&AUnrealTestPlayerStart::FreePoint,
		SpawnOccupyDuration);
}

void AUnrealTestPlayerStart::FreePoint()
{
	bIsOccupied = false;
}


