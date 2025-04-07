#include "WeaponActor.h"

#include "AimingEffect/PlayerAimingEffect.h"
#include "Components/PawnNoiseEmitterComponent.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Prepper/Prepper.h"
#include "Prepper/GamePlay/Character/BCharacter.h"
#include "Prepper/GamePlay/CharacterController/CharacterController.h"
#include "Prepper/GamePlay/CharacterController/Combat.h"
#include "Prepper/GamePlay/Weapon/Ammo/WeaponMagazine.h"
#include "Prepper/GamePlay/Weapon/Attack/WeaponAttacking.h"
#include "Prepper/GamePlay/Weapon/Targeting/WeaponTargeting.h"
#include "Prepper/__Legacy/Character/PlayerCharacter.h"
#include "Prepper/__Legacy/HUD/PrepperHUD.h"
#include "Prepper/__Legacy/PlayerController/BasePlayerController.h"
#include "Sound/SoundCue.h"

AWeaponActor::AWeaponActor()
{
	CustomDepthColor = CUSTOM_DEPTH_MINT;

	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	AActor::SetReplicateMovement(true);

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);

	WeaponMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	WeaponMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	StaticWeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeleeWeaponMesh"));
	StaticWeaponMesh->SetupAttachment(RootComponent);
	StaticWeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	WeaponMesh->SetCustomDepthStencilValue(CustomDepthColor);
	StaticWeaponMesh->SetCustomDepthStencilValue(CustomDepthColor);
	WeaponMesh->MarkRenderStateDirty();
	StaticWeaponMesh->MarkRenderStateDirty();
	StaticWeaponMesh->SetSimulatePhysics(false);
	StaticWeaponMesh->SetEnableGravity(false);

	AreaBox = CreateDefaultSubobject<UBoxComponent>("AreaBox");
	AreaBox->SetupAttachment(RootComponent);
	AreaBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ToggleTrigger(true);
	AreaBox->SetCollisionObjectType(ECC_InteractMesh);

	PickUpWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickUpWidget"));
	PickUpWidget->SetupAttachment(RootComponent);

	Targeting = CreateDefaultSubobject<UWeaponTargeting>("WeaponTargeting");
	Attacking = CreateDefaultSubobject<UWeaponAttacking>("WeaponAttacking");
	Magazine = CreateDefaultSubobject<UWeaponMagazine>("WeaponMagazine");

	Muzzle = CreateDefaultSubobject<USceneComponent>("Muzzle");
	Muzzle->AttachToComponent(GetRootComponent(),
	                          FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));

	// 노이즈 생성 컴포넌트 추가
	PawnNoiseEmitter = CreateDefaultSubobject<UPawnNoiseEmitterComponent>(TEXT("PawnNoiseEmitter"));
}

void AWeaponActor::WeaponPhysicsActive(bool bActive)
{
	SetActorEnableCollision(bActive);

	WeaponMesh->SetSimulatePhysics(bActive);
	WeaponMesh->SetEnableGravity(bActive);

	ToggleOutline(bActive);
	ToggleTrigger(bActive);

	if (!bActive)
	{
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		return;
	}

	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	WeaponMesh->SetCollisionResponseToChannels(ColliderChannel);
}

TArray<UPlayerAimingEffect*> AWeaponActor::GetAimingEffect()
{
	if (AimingEffectClasses.Num() != AimingEffects.Num())
	{
		for (TSubclassOf<UPlayerAimingEffect> EffectClass : AimingEffectClasses)
		{
			AimingEffects.Add(NewObject<UPlayerAimingEffect>(this, EffectClass));
		}
	}
	return AimingEffects;
}

void AWeaponActor::PlayEquipWeaponSound(const AActor* TargetActor) const
{
	if (!EquipSound) return;
	if (!TargetActor) return;

	UGameplayStatics::PlaySoundAtLocation(
		TargetActor, EquipSound,
		TargetActor->GetActorLocation()
	);
}

void AWeaponActor::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority()) return;
	
	AreaBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	AreaBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SetWeaponState(EWeaponState::EWS_Initial);
	SetWeaponState(EWeaponState::EWS_Dropped);
	
}

void AWeaponActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeaponActor, WeaponState);
	DOREPLIFETIME_CONDITION(AWeaponActor, bUseServerSideRewind, COND_OwnerOnly);
}

void AWeaponActor::SetWeaponHandler(IWeaponHandler* NewOwner)
{
	WeaponHandler = NewOwner;
}

void AWeaponActor::Interaction(APlayerCharacter* Target)
{
	Target->EquipWeapon(this);
}

void AWeaponActor::Interaction(ICharacterController* Target)
{
	Target->GetCombat()->EquipWeapon(this);
}

bool AWeaponActor::CanAttack()
{
	return Magazine->CanAttack();
}

int AWeaponActor::GetLeftAmmo() const
{
	return Magazine->GetLeftAmmo();
}

// client
void AWeaponActor::OnRep_WeaponState()
{
	OnWeaponStateSet();
}

void AWeaponActor::GetCrosshair(float DeltaTime, bool bIsAiming, FHUDPackage& Crosshair)
{
	Crosshair.CrosshairCenter = nullptr;
	Crosshair.CrosshairLeft = nullptr;
	Crosshair.CrosshairRight = nullptr;
	Crosshair.CrosshairTop = nullptr;
	Crosshair.CrosshairBottom = nullptr;
	Crosshair.CrosshairSpread = 0.5f;
}

void AWeaponActor::OnWeaponStateSet()
{
	switch (WeaponState)
	{
	case EWeaponState::EWS_Equipped:
		OnEquipped(Cast<ABaseCharacter>(GetOwner()));
		break;
	case EWeaponState::EWS_Holstered:
		OnEquippedSecondary(Cast<ABaseCharacter>(GetOwner()));
		break;
	case EWeaponState::EWS_Dropped:
		OnDropped(Cast<ABaseCharacter>(GetOwner()));
		break;
	default:
		break;
	}
}

void AWeaponActor::OnEquipped(ABCharacter* TargetCharacter)
{
	ShowPickUpWidget(false);
	WeaponPhysicsActive(false);;

	if (!TargetCharacter) return;

	TargetCharacter->AttachActorAtSocket(WeaponSocketName, this);
	TargetCharacter->SetEquippedWeaponType(WeaponType);
	PlayEquipWeaponSound(TargetCharacter);
}

void AWeaponActor::OnDropped(ABCharacter* TargetCharacter)
{
	const FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);

	WeaponPhysicsActive(true);
	WeaponMesh->DetachFromComponent(DetachRules);
}

void AWeaponActor::OnEquippedSecondary(ABCharacter* TargetCharacter)
{
	ShowPickUpWidget(false);
	WeaponPhysicsActive(false);

	if (!TargetCharacter) return;

	TargetCharacter->AttachActorAtSocket(HolsteredWeaponSocketName, this);
	PlayEquipWeaponSound(TargetCharacter);
}

TArray<FVector_NetQuantize> AWeaponActor::GetTarget(FVector& HitTarget) const
{
	return Targeting->GetTarget(HitTarget);
}

void AWeaponActor::Fire(const TArray<FVector_NetQuantize>& HitTargets, AController* Attacker,
						const bool IsSimulate) const
{
	Attacking->Fire(Muzzle->GetComponentLocation(), HitTargets, Attacker, IsSimulate);
	Magazine->UseAmmo(1);
}

void AWeaponActor::OnEquipped(ABaseCharacter* TargetCharacter)
{
	ShowPickUpWidget(false);
	WeaponPhysicsActive(false);

	if (!TargetCharacter) return;

	TargetCharacter->AttachActorAtSocket(WeaponSocketName, this);
	PlayEquipWeaponSound(TargetCharacter);
	UE_LOG(LogTemp, Warning, TEXT("WEAPON : WEAPON EQUIPPED"));

	if (!bUseServerSideRewind) return;

	PlayerOwnerController = PlayerOwnerController == nullptr
		                        ? Cast<ABasePlayerController>(TargetCharacter->Controller)
		                        : PlayerOwnerController;

	if (PlayerOwnerController && HasAuthority() && !PlayerOwnerController->HighPingDelegate.IsBound())
	{
		PlayerOwnerController->HighPingDelegate.AddDynamic(this, &AWeaponActor::OnPingTooHigh);
	}
}

void AWeaponActor::OnDropped(ABaseCharacter* TargetCharacter)
{
	UE_LOG(LogTemp, Warning, TEXT("WEAPON : WEAPON DROPPED"));
	const FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);

	WeaponPhysicsActive(true);
	WeaponMesh->DetachFromComponent(DetachRules);

	SetOwner(nullptr);

	if (!TargetCharacter) return;

	PlayerOwnerController = PlayerOwnerController == nullptr
		                        ? Cast<ABasePlayerController>(TargetCharacter->Controller)
		                        : PlayerOwnerController;

	PlayerOwnerController = nullptr;
	if (PlayerOwnerController && HasAuthority() && PlayerOwnerController->HighPingDelegate.IsBound())
	{
		PlayerOwnerController->HighPingDelegate.RemoveDynamic(this, &AWeaponActor::OnPingTooHigh);
	}
}

void AWeaponActor::OnEquippedSecondary(ABaseCharacter* TargetCharacter)
{
	ShowPickUpWidget(false);
	WeaponPhysicsActive(false);

	if (!TargetCharacter) return;

	UE_LOG(LogTemp, Warning, TEXT("WEAPON : WEAPON SECONDARY"));
	TargetCharacter->AttachActorAtSocket(HolsteredWeaponSocketName, this);
	PlayEquipWeaponSound(TargetCharacter);

	PlayerOwnerController = PlayerOwnerController == nullptr
		                        ? Cast<ABasePlayerController>(TargetCharacter->Controller)
		                        : PlayerOwnerController;
	if (PlayerOwnerController && HasAuthority() && PlayerOwnerController->HighPingDelegate.IsBound())
	{
		PlayerOwnerController->HighPingDelegate.RemoveDynamic(this, &AWeaponActor::OnPingTooHigh);
	}
}

IWeaponHandler* AWeaponActor::GetWeaponHandler()
{
	if (WeaponHandler != nullptr) return WeaponHandler;

	return nullptr;
}

void AWeaponActor::OnPingTooHigh(bool bPingTooHigh)
{
	bUseServerSideRewind = !bPingTooHigh;
}

bool AWeaponActor::CanReload()
{
	return true;
}

void AWeaponActor::SetWeaponState(EWeaponState State)
{
	WeaponState = State;
	OnWeaponStateSet();
}
