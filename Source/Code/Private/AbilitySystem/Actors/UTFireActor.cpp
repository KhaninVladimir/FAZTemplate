// Copyright 2022 Khanin Vladimir (FAZIC)


#include "AbilitySystem/Actors/UTFireActor.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
AUTFireActor::AUTFireActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(FName("ProjectileMovement"));
}

// Called when the game starts or when spawned
void AUTFireActor::BeginPlay()
{
	Super::BeginPlay();

}
