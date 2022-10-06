// Copyright Epic Games, Inc. All Rights Reserved.

#include "Character/UnrealTestCharacter.h"
#include "Character/Component/UTCharacterMovementComponent.h"
#include "Character/Component/UTHealthComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

#include "Common/UnrealTestType.h"

#include "Game/UnrealTestGameMode.h"
#include "Game/UnrealTestGameState.h"

#include "Player/UnrealTestPlayerState.h"
#include "Player/UnrealTestPlayerController.h"

#include "System/UnrealTestGameplayTags.h"
#include "AbilitySystem/UnrealTestAbilitySystemComponent.h"
#include "AbilitySystem/Attribute/UnrealTestAttributeSet.h"
#include "AbilitySystemGlobals.h"
#include "GameplayAbilitySpec.h"


//////////////////////////////////////////////////////////////////////////
// AUnrealTestCharacter

AUnrealTestCharacter::AUnrealTestCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UUTCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rate for input
	TurnRateGamepad = TURN_RATE_GAMEPAD;

	DisableCotrollerRotation();

	ConfigureCharacterMovement(GetCharacterMovement());
	
	SetCameraBoom();
	SetFollowCamera();

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	HealthComponent = CreateDefaultSubobject<UUTHealthComponent>(TEXT("HealthComponent"));
	HealthComponent->OnDying.AddDynamic(this, &ThisClass::CharacterDied);
}

void AUnrealTestCharacter::DisableCotrollerRotation()
{
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
}

void AUnrealTestCharacter::ConfigureCharacterMovement(UCharacterMovementComponent* characterMovement)
{
	// Configure character movement
	characterMovement->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	characterMovement->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	characterMovement->JumpZVelocity = JUMP_Z_VELOCITY;
	characterMovement->AirControl = AIR_CONTROL;
	characterMovement->MaxWalkSpeed = MAX_WALK_SPEED;
	characterMovement->MinAnalogWalkSpeed = MIN_ANALOG_WALK_SPEED;
	characterMovement->BrakingDecelerationWalking = BRAKING_DECELERATION_WALKING;
}

void AUnrealTestCharacter::SetCameraBoom()
{
	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
}

void AUnrealTestCharacter::SetFollowCamera()
{
	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
}

//////////////////////////////////////////////////////////////////////////
// Input

void AUnrealTestCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);

	JumpBinding(PlayerInputComponent);

	MovementBinding(PlayerInputComponent);

	TurnBinding(PlayerInputComponent);
	LookUpBinding(PlayerInputComponent);
	
	TouchBinding(PlayerInputComponent);

	// Bind player input to the AbilitySystemComponent. Also called in OnRep_PlayerState because of a potential race condition.
	BindASCInput();
}

void AUnrealTestCharacter::JumpBinding(class UInputComponent* PlayerInputComponent)
{
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
}

void AUnrealTestCharacter::MovementBinding(class UInputComponent* PlayerInputComponent)
{
	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &AUnrealTestCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &AUnrealTestCharacter::MoveRight);
}

void AUnrealTestCharacter::TurnBinding(class UInputComponent* PlayerInputComponent)
{
	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &AUnrealTestCharacter::TurnAtRate);
}

void AUnrealTestCharacter::LookUpBinding(class UInputComponent* PlayerInputComponent)
{
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &AUnrealTestCharacter::LookUpAtRate);
}

void AUnrealTestCharacter::TouchBinding(class UInputComponent* PlayerInputComponent)
{
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AUnrealTestCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AUnrealTestCharacter::TouchStopped);
}

void AUnrealTestCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void AUnrealTestCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

void AUnrealTestCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void AUnrealTestCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void AUnrealTestCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AUnrealTestCharacter::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void AUnrealTestCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (auto PS = GetUTPlayerState())
	{
		PS->SetPlayerPhase(EPlayerPhase::PP_Battle);
	}

	// Initialize our abilities
	if (auto ASC = GetUTAbilitySystemComponent())
	{
		ASC->InitAbilityActorInfo(this, this);

		AddCharacterAbilities();
		InitializeAttributes();

		HealthComponent->InitializeWithAbilitySystem(ASC);
	}
}

void AUnrealTestCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (auto ASC = GetUTAbilitySystemComponent())
	{
		ASC->InitAbilityActorInfo(this, this);

		InitializeAttributes();
		BindASCInput();
		HealthComponent->InitializeWithAbilitySystem(ASC);
	}

	K2_OnRep_PlayerState();
}

void AUnrealTestCharacter::AddCharacterAbilities()
{
	if (!HasAuthority())
		return;

	auto ASC = GetUTAbilitySystemComponent();
	if (!ASC)
		return;

	if (ASC->CharacterAbilitiesGiven)
		return;

	for (auto StartupAbility : CharacterAbilities)
	{
		FGameplayAbilitySpec AbilitySpec(StartupAbility, 1, static_cast<int32>(StartupAbility.GetDefaultObject()->AbilityInputID));

		ASC->GiveAbility(AbilitySpec);
	}

	ASC->CharacterAbilitiesGiven = true;
}

void AUnrealTestCharacter::RemoveCharacterAbilities()
{
	if (!HasAuthority())
		return;

	auto ASC = GetUTAbilitySystemComponent();
	if (!ASC)
		return;

	if (!ASC->CharacterAbilitiesGiven)
		return;

	// Remove any abilities added from a previous call. This checks to make sure the ability is in the startup 'CharacterAbilities' array.
	TArray<FGameplayAbilitySpecHandle> AbilitiesToRemove;
	for (const FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
	{
		if (CharacterAbilities.Contains(Spec.Ability->GetClass()))
		{
			AbilitiesToRemove.Add(Spec.Handle);
		}
	}

	// Do in two passes so the removal happens after we have the full list
	for (int32 i = 0; i < AbilitiesToRemove.Num(); i++)
	{
		ASC->ClearAbility(AbilitiesToRemove[i]);
	}

	ASC->CharacterAbilitiesGiven = false;
}


void AUnrealTestCharacter::InitializeAttributes()
{
	auto ASC = GetAbilitySystemComponent();
	if (!ASC)
		return;

	if (!DefaultAttributes)
	{
		UE_LOG(LogTemp, Error, TEXT("%s() Missing DefaultAttributes for %s. Please fill in the character's Blueprint."), *FString(__FUNCTION__), *GetName());
		return;
	}

	// Can run on Server and Client
	FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	FGameplayEffectSpecHandle NewHandle = ASC->MakeOutgoingSpec(DefaultAttributes, 1, EffectContext);
	if (NewHandle.IsValid())
		FActiveGameplayEffectHandle ActiveGEHandle = ASC->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), ASC);
}

void AUnrealTestCharacter::BindASCInput()
{
	if (!bASCInputBound && AbilitySystemComponentPtr.IsValid() && IsValid(InputComponent))
	{
		AbilitySystemComponentPtr->BindAbilityActivationToInputComponent(InputComponent, FGameplayAbilityInputBinds(FString("ConfirmTarget"),
			FString("CancelTarget"), FString("EUTAbilityInputID"), static_cast<int32>(EUTAbilityInputID::Confirm), static_cast<int32>(EUTAbilityInputID::Cancel)));

		bASCInputBound = true;
	}
}

void AUnrealTestCharacter::CharacterDied(float DamageAmount, AActor* EventInstigator, AActor* DamageCauser)
{
#if WITH_SERVER_CODE
	AController* ControllerInstigator = Cast<AController>(EventInstigator->GetInstigatorController());

	FUTKilledInfo info;
	FUTKillerPlayerInfo KillerInfo;
	FUTKillerPlayerInfo VictimInfo;
	VictimInfo.PlayerName = GetCharacterName();
	int32 VictimStreak = 0;
	int32 KillerStreak = 0;
	AUnrealTestPlayerState* VictimPS = GetUTPlayerState();
	AUnrealTestPlayerState* KillerPS = ControllerInstigator ? Cast<AUnrealTestPlayerState>(ControllerInstigator->PlayerState) : nullptr;
	if (VictimPS && KillerPS && VictimPS != KillerPS)
	{
		if (KillerPS->PlayerKillStat.Contains(VictimPS))
		{
			KillerStreak = KillerPS->PlayerKillStat.FindRef(VictimPS);
		}
		KillerStreak++;
		KillerPS->PlayerKillStat.Add(VictimPS, KillerStreak);
		if (VictimPS->PlayerKillStat.Contains(KillerPS))
		{
			VictimStreak = VictimPS->PlayerKillStat.FindRef(KillerPS);
		}
		VictimInfo.KillCount = VictimStreak;
	}

	AUnrealTestCharacter* KillerPlayer = ControllerInstigator ? Cast<AUnrealTestCharacter>(ControllerInstigator->GetPawn()) : nullptr;
	if (KillerPlayer)
	{
		KillerInfo.PlayerName = KillerPlayer->GetCharacterName();
		KillerInfo.KillCount = KillerStreak;
	}
	info.KillerPlayer = KillerInfo;
	info.VictimPlayer = VictimInfo;
	info.DamageCauser = DamageCauser;
	info.DamageCount = DamageAmount;

	if (auto PC = Cast<AUnrealTestPlayerController>(GetController()))
		PC->Client_SendKilledInfo(info);

	if (auto PC = Cast<AUnrealTestPlayerController>(ControllerInstigator))
		PC->Client_SendKilledInfo(info);

	Die(ControllerInstigator, DamageCauser);
#endif // #if WITH_SERVER_CODE
}

void AUnrealTestCharacter::Die(AController* Killer, AActor* DamageCauser, bool bSimple)
{
#if WITH_SERVER_CODE
	if (!HasAuthority())
		return;

	if (AUnrealTestGameMode* GM = GetUTGameMode())
	{
		AController* VictimPlayer = Controller ? Controller : Cast<AController>(GetOwner());
		GM->Killed(LastHitBy ? LastHitBy : Killer, VictimPlayer, this, DamageCauser);
	}

	RemoveCharacterAbilities();

	DisableMovementAndCollision();
	OnDeath();
#endif // #if WITH_SERVER_CODE
}

void AUnrealTestCharacter::OnDeath()
{
	ActivateRagdollPhysics();

	if (auto ACS = GetUTAbilitySystemComponent())
	{
		ACS->CancelAllAbilities();

		const FUnrealTestGameplayTags& GameplayTags = FUnrealTestGameplayTags::Get();

		FGameplayTagContainer EffectTagsToRemove;
		EffectTagsToRemove.AddTag(GameplayTags.State_Dead);
		int32 NumEffectsRemoved = ACS->RemoveActiveEffectsWithTags(EffectTagsToRemove);
	}
}

void AUnrealTestCharacter::DisableMovementAndCollision()
{
	if (Controller)
		Controller->SetIgnoreMoveInput(true);	
	
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::DestroyDueToDeath);
}

void AUnrealTestCharacter::ActivateRagdollPhysics()
{
	auto mesh = GetMesh();
	if (!mesh)
		return;
	
	mesh->SetCollisionProfileName(TEXT("Ragdoll"));
	if (!mesh->GetPhysicsAsset())
		return;

	mesh->SetAllBodiesSimulatePhysics(true);
	mesh->SetSimulatePhysics(true);
	mesh->WakeAllRigidBodies();
	mesh->bBlendPhysics = true;

	if (auto movement = GetCharacterMovementComponent())
	{
		movement->StopMovementImmediately();
		movement->DisableMovement();
		movement->SetComponentTickEnabled(false);
	}
}

void AUnrealTestCharacter::DestroyDueToDeath()
{
	if (HasAuthority())
		DetachFromControllerPendingDestroy();
}

bool AUnrealTestCharacter::IsAlive() const
{
	if (HealthComponent)
		return HealthComponent->IsAlive();

	return true;
}

void AUnrealTestCharacter::DetachFromControllerPendingDestroy()
{
	if (HasAuthority())
	{
		SetLifeSpan(1.f);
	}
	Super::DetachFromControllerPendingDestroy();
}

FString AUnrealTestCharacter::GetCharacterName() const
{
	if (auto PS = GetPlayerState())
	{
		return GetPlayerState()->GetPlayerName();
	}
	return "Unknown";
}

FGenericTeamId AUnrealTestCharacter::GetGenericTeamId() const
{
	if (auto PS = GetUTPlayerState())
		return PS->GetTeamID();

	return 255;
}

const UUnrealTestAttributeSet* AUnrealTestCharacter::GetAttributeSet() const
{
	if (!IsValid(AttributeSetPtr))
		AttributeSetPtr = GetAbilitySystemComponent()->GetSet<UUnrealTestAttributeSet>();

	return AttributeSetPtr;
}

UAbilitySystemComponent* AUnrealTestCharacter::GetAbilitySystemComponent() const
{
	if (!AbilitySystemComponentPtr.IsValid() && GetPlayerState())
		AbilitySystemComponentPtr = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetPlayerState());

	return AbilitySystemComponentPtr.Get();
}

UUnrealTestAbilitySystemComponent* AUnrealTestCharacter::GetUTAbilitySystemComponent() const
{
	return Cast<UUnrealTestAbilitySystemComponent>(GetAbilitySystemComponent());
}

UUTCharacterMovementComponent* AUnrealTestCharacter::GetCharacterMovementComponent() const
{
	if (!CharacterMovementPtr.IsValid())
		CharacterMovementPtr = Cast<UUTCharacterMovementComponent>(GetCharacterMovement());

	return CharacterMovementPtr.Get();
}

AUnrealTestGameState* AUnrealTestCharacter::GetUTGameState() const
{
	if (!GameStatePtr.IsValid() && GetWorld())
		GameStatePtr = GetWorld()->GetGameState<AUnrealTestGameState>();

	return GameStatePtr.Get();
}

AUnrealTestGameMode* AUnrealTestCharacter::GetUTGameMode() const
{
	if (!GameModePtr.IsValid() && GetWorld())
		GameModePtr = GetWorld()->GetAuthGameMode<AUnrealTestGameMode>();

	return GameModePtr.Get();
}

AUnrealTestPlayerState* AUnrealTestCharacter::GetUTPlayerState() const
{
	if (!PlayerStatePtr.IsValid() && GetPlayerState())
		PlayerStatePtr = Cast<AUnrealTestPlayerState>(GetPlayerState());

	return PlayerStatePtr.Get();
}