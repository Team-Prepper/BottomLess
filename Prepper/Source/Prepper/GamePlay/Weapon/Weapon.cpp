#include "Weapon.h"

#include "AimingEffect/PlayerAimingEffect.h"
#include "Components/PawnNoiseEmitterComponent.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Prepper/Prepper.h"
#include "Prepper/GamePlay/Character/BCharacter.h"
#include "Prepper/GamePlay/Character/Component/BCombatComponent.h"
#include "Prepper/GamePlay/CharacterController/CharacterController.h"
#include "Prepper/GamePlay/Weapon/Ammo/WeaponMagazine.h"
#include "Prepper/GamePlay/Weapon/Attack/WeaponAttacking.h"
#include "Prepper/GamePlay/Weapon/Targeting/WeaponTargeting.h"
#include "Prepper/___Legacy/Character/PlayerCharacter.h"
#include "Prepper/___Legacy/HUD/PrepperHUD.h"
#include "Prepper/___Legacy/PlayerController/BasePlayerController.h"
#include "Sound/SoundCue.h"

AWeapon::AWeapon()
{
	CustomDepthColor = CUSTOM_DEPTH_MINT;

	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	AActor::SetReplicateMovement(true);

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	WeaponMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetCustomDepthStencilValue(CustomDepthColor);
	WeaponMesh->MarkRenderStateDirty();
	
	SetRootComponent(WeaponMesh);

	StaticWeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeleeWeaponMesh"));
	StaticWeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StaticWeaponMesh->SetCustomDepthStencilValue(CustomDepthColor);
	StaticWeaponMesh->MarkRenderStateDirty();
	StaticWeaponMesh->SetSimulatePhysics(false);
	StaticWeaponMesh->SetEnableGravity(false);
	StaticWeaponMesh->SetupAttachment(RootComponent);

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

void AWeapon::WeaponPhysicsActive(bool bActive)
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

TArray<UPlayerAimingEffect*> AWeapon::GetAimingEffect()
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

void AWeapon::PlayEquipWeaponSound(const AActor* TargetActor) const
{
	if (!EquipSound) return;
	if (!TargetActor) return;

	UGameplayStatics::PlaySoundAtLocation(
		TargetActor, EquipSound,
		TargetActor->GetActorLocation()
	);
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority()) return;
	
	AreaBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	AreaBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SetWeaponState(EWeaponState::EWS_Initial);
	SetWeaponState(EWeaponState::EWS_Dropped);
	
}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeapon, WeaponState);
	DOREPLIFETIME_CONDITION(AWeapon, bUseServerSideRewind, COND_OwnerOnly);
}

void AWeapon::SetWeaponHandler(IWeaponHandler* NewOwner)
{
	WeaponHandler = NewOwner;
}

void AWeapon::PlayReload(const TObjectPtr<ABCharacter> TargetCharacter, TObjectPtr<UAnimMontage> ReloadMontage) const
{
	TargetCharacter->PlayAnim(ReloadMontage, ReloadActionName);
}

void AWeapon::PlayReload(const TObjectPtr<ABaseCharacter> TargetCharacter, const TObjectPtr<UAnimMontage> ReloadMontage) const
{
	TargetCharacter->PlayAnim(ReloadMontage, ReloadActionName);
}

FString AWeapon::GetAmmoValue()
{
	return Magazine->ToString();
}

void AWeapon::Interaction(APlayerCharacter* Target)
{
	Target->EquipWeapon(this);
}

void AWeapon::Interaction(ICharacterController* Target)
{
	Target->GetCombat()->EquipWeapon(this);
}

void AWeapon::SetStateAiming(const TObjectPtr<ABCharacter> TargetCharacter)
{
	for (UPlayerAimingEffect* Effect : GetAimingEffect())
	{
		Effect->CharacterAimingStart(TargetCharacter);
	}
}

void AWeapon::SetStateUnAiming()
{
	for (UPlayerAimingEffect* Effect : GetAimingEffect())
	{
		Effect->PlayerAimingEnd();
		Effect->CharacterAimingEnd();
	}
}

void AWeapon::SetStateAiming(TObjectPtr<APlayerCharacter> TargetCharacter)
{
	for (UPlayerAimingEffect* Effect : GetAimingEffect())
	{
		Effect->PlayerAimingStart(TargetCharacter);
	}
}

bool AWeapon::CanAttack()
{
	return Magazine->CanAttack();
}

int AWeapon::GetLeftAmmo() const
{
	return Magazine->GetLeftAmmo();
}

void AWeapon::SetLeftAmmo(const int Amount) const
{
	Magazine->SetAmmo(Amount);
}

bool AWeapon::CanReload()
{
	return Magazine->CanReload();
}

void AWeapon::Reload(IAmmoBox* AmmoBox)
{
	Magazine->Reload(AmmoBox, WeaponType);
}

// client
void AWeapon::OnRep_WeaponState()
{
	OnWeaponStateSet();
}

void AWeapon::GetCrosshair(float DeltaTime, bool bIsAiming, FHUDPackage& Crosshair)
{
	Crosshair.CrosshairCenter = nullptr;
	Crosshair.CrosshairLeft = nullptr;
	Crosshair.CrosshairRight = nullptr;
	Crosshair.CrosshairTop = nullptr;
	Crosshair.CrosshairBottom = nullptr;
	Crosshair.CrosshairSpread = 0.5f;
}

void AWeapon::OnWeaponStateSet()
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

void AWeapon::OnEquipped(const TObjectPtr<ABCharacter> TargetCharacter)
{
	ShowPickUpWidget(false);
	WeaponPhysicsActive(false);

	if (!TargetCharacter) return;
	
	TargetCharacter->SetEquippedWeaponType(WeaponType);
	TargetCharacter->AttachActorAtSocket(WeaponSocketName, this);
	
	PlayEquipWeaponSound(TargetCharacter);
}

void AWeapon::OnDropped(TObjectPtr<ABCharacter> TargetCharacter)
{
	const FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);

	WeaponPhysicsActive(true);
	WeaponMesh->DetachFromComponent(DetachRules);
}

void AWeapon::OnEquippedSecondary(TObjectPtr<ABCharacter> TargetCharacter)
{
	ShowPickUpWidget(false);
	WeaponPhysicsActive(false);

	if (!TargetCharacter) return;

	TargetCharacter->AttachActorAtSocket(HolsteredWeaponSocketName, this);
	PlayEquipWeaponSound(TargetCharacter);
}

TArray<FVector_NetQuantize> AWeapon::GetTarget(FVector& HitTarget) const
{
	return Targeting->GetTarget(HitTarget);
}

void AWeapon::Fire(const TArray<FVector_NetQuantize>& HitTargets, AController* Attacker,
						const bool IsSimulate) const
{
	Attacking->Fire(Muzzle->GetComponentLocation(), HitTargets, Attacker, IsSimulate);
	Magazine->UseAmmo(1);
}

void AWeapon::OnEquipped(ABaseCharacter* TargetCharacter)
{
	ShowPickUpWidget(false);
	WeaponPhysicsActive(false);

	if (!TargetCharacter) return;
	
	//SetOwner(TargetCharacter);
	TargetCharacter->AttachActorAtSocket(WeaponSocketName, this);
	PlayEquipWeaponSound(TargetCharacter);
	UE_LOG(LogTemp, Warning, TEXT("WEAPON : WEAPON EQUIPPED"));

	if (!bUseServerSideRewind) return;

	PlayerOwnerController = PlayerOwnerController == nullptr
		                        ? Cast<ABasePlayerController>(TargetCharacter->Controller)
		                        : PlayerOwnerController;

	if (PlayerOwnerController && HasAuthority() && !PlayerOwnerController->HighPingDelegate.IsBound())
	{
		PlayerOwnerController->HighPingDelegate.AddDynamic(this, &AWeapon::OnPingTooHigh);
	}
}

void AWeapon::OnDropped(ABaseCharacter* TargetCharacter)
{
	UE_LOG(LogTemp, Warning, TEXT("WEAPON : WEAPON DROPPED"));
	const FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);

	WeaponPhysicsActive(true);
	WeaponMesh->DetachFromComponent(DetachRules);

	//SetOwner(nullptr);

	if (!TargetCharacter) return;

	PlayerOwnerController = PlayerOwnerController == nullptr
		                        ? Cast<ABasePlayerController>(TargetCharacter->Controller)
		                        : PlayerOwnerController;

	PlayerOwnerController = nullptr;
	if (PlayerOwnerController && HasAuthority() && PlayerOwnerController->HighPingDelegate.IsBound())
	{
		PlayerOwnerController->HighPingDelegate.RemoveDynamic(this, &AWeapon::OnPingTooHigh);
	}
}

void AWeapon::OnEquippedSecondary(ABaseCharacter* TargetCharacter)
{
	ShowPickUpWidget(false);
	WeaponPhysicsActive(false);

	if (!TargetCharacter) return;

	//SetOwner(TargetCharacter);
	
	UE_LOG(LogTemp, Warning, TEXT("WEAPON : WEAPON SECONDARY"));
	TargetCharacter->AttachActorAtSocket(HolsteredWeaponSocketName, this);
	PlayEquipWeaponSound(TargetCharacter);

	PlayerOwnerController = PlayerOwnerController == nullptr
		                        ? Cast<ABasePlayerController>(TargetCharacter->Controller)
		                        : PlayerOwnerController;
	if (PlayerOwnerController && HasAuthority() && PlayerOwnerController->HighPingDelegate.IsBound())
	{
		PlayerOwnerController->HighPingDelegate.RemoveDynamic(this, &AWeapon::OnPingTooHigh);
	}
}

IWeaponHandler* AWeapon::GetWeaponHandler()
{
	if (WeaponHandler != nullptr) return WeaponHandler;

	return nullptr;
}

void AWeapon::OnPingTooHigh(bool bPingTooHigh)
{
	bUseServerSideRewind = !bPingTooHigh;
}

void AWeapon::SetWeaponState(EWeaponState State)
{
	WeaponState = State;
	OnWeaponStateSet();
}
