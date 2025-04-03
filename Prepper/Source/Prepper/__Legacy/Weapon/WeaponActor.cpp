#include "WeaponActor.h"

#include "AimingEffect/PlayerAimingEffect.h"
#include "Components/PawnNoiseEmitterComponent.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Prepper/Prepper.h"
#include "Prepper/GamePlay/Weapon/Targeting/WeaponTargeting.h"
#include "Prepper/__Legacy/Character/PlayerCharacter.h"
#include "Prepper/__Legacy/PlayerController/BasePlayerController.h"
#include "Sound/SoundCue.h"

AWeaponActor::AWeaponActor()
{
	CustomDepthColor = CUSTOM_DEPTH_MINT;
	
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicateMovement(true);

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

	// 노이즈 생성 컴포넌트 추가
	PawnNoiseEmitter = CreateDefaultSubobject<UPawnNoiseEmitterComponent>(TEXT("PawnNoiseEmitter"));
	
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

void AWeaponActor::PlayEquipWeaponSound()
{
	if (!EquipSound) return;
	if (!OwnerCharacter) return;
	
	UGameplayStatics::PlaySoundAtLocation(
		OwnerCharacter, EquipSound,
		OwnerCharacter->GetActorLocation()
	);
}

void AWeaponActor::BeginPlay()
{
	Super::BeginPlay();
	
	if(HasAuthority())
	{
		AreaBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        AreaBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
		SetWeaponState(EWeaponState::EWS_Initial);
		SetWeaponState(EWeaponState::EWS_Dropped);
	}
}

void AWeaponActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeaponActor, WeaponState);
	DOREPLIFETIME_CONDITION(AWeaponActor, bUseServerSideRewind, COND_OwnerOnly);
}

void AWeaponActor::OnRep_Owner()
{
	Super::OnRep_Owner();
	if(Owner == nullptr)
	{
		OwnerCharacter = nullptr;
		PlayerOwnerController = nullptr;
		WeaponHandler = nullptr;
		return;
	}
}

void AWeaponActor::SetWeaponHandler(IWeaponHandler* NewOwner)
{
	WeaponHandler = NewOwner;
}

void AWeaponActor::Interaction(APlayerCharacter* Target)
{
	Target->EquipWeapon(this);
}
void AWeaponActor::SetWeaponState(EWeaponState State)
{
	WeaponState = State;
	OnWeaponStateSet();
}

// client
void AWeaponActor::OnRep_WeaponState()
{
	OnWeaponStateSet();
}

void AWeaponActor::OnWeaponStateSet()
{
	switch (WeaponState)
	{
		case EWeaponState::EWS_Equipped:
			OnEquipped();
			break;
		case EWeaponState::EWS_Holstered:
			OnEquippedSecondary();
			break;
		case EWeaponState::EWS_Dropped:
			OnDropped();
			break;
		default:
			break;
	}
}

void AWeaponActor::OnEquipped()
{
	ShowPickUpWidget(false);
	WeaponPhysicsActive(false);

	OwnerCharacter = OwnerCharacter == nullptr ?
		Cast<ABaseCharacter>(GetOwner()) : OwnerCharacter;

	if (!OwnerCharacter) return;

	OwnerCharacter->AttachActorAtSocket(AttachSocketName(), this);
	PlayEquipWeaponSound();
	UE_LOG(LogTemp, Warning , TEXT("WEAPON : WEAPON EQUIPPED"));
		
	if (!bUseServerSideRewind) return;
	
	PlayerOwnerController = PlayerOwnerController == nullptr ?
		Cast<ABasePlayerController>(OwnerCharacter->Controller) : PlayerOwnerController;
	
	if (PlayerOwnerController && HasAuthority() && !PlayerOwnerController->HighPingDelegate.IsBound())
	{
		PlayerOwnerController->HighPingDelegate.AddDynamic(this, &AWeaponActor::OnPingTooHigh);
	}
}

void AWeaponActor::OnDropped()
{
	UE_LOG(LogTemp, Warning , TEXT("WEAPON : WEAPON DROPPED"));
	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);

	WeaponPhysicsActive(true);
	WeaponMesh->DetachFromComponent(DetachRules);
	
	SetOwner(nullptr);

	OwnerCharacter = OwnerCharacter == nullptr ?
		Cast<ABaseCharacter>(GetOwner()) : OwnerCharacter;
	
	if (!OwnerCharacter) return;
	
	PlayerOwnerController = PlayerOwnerController == nullptr ?
		Cast<ABasePlayerController>(OwnerCharacter->Controller) : PlayerOwnerController;
	OwnerCharacter = nullptr;
	
	PlayerOwnerController = nullptr;
	if (PlayerOwnerController && HasAuthority() && PlayerOwnerController->HighPingDelegate.IsBound())
	{
		PlayerOwnerController->HighPingDelegate.RemoveDynamic(this, &AWeaponActor::OnPingTooHigh);
	}
}

void AWeaponActor::OnEquippedSecondary()
{
	ShowPickUpWidget(false);
	WeaponPhysicsActive(false);
	
	OwnerCharacter = OwnerCharacter == nullptr ?
		Cast<ABaseCharacter>(GetOwner()) : OwnerCharacter;

	if (!OwnerCharacter) return;
	
	UE_LOG(LogTemp, Warning , TEXT("WEAPON : WEAPON SECONDARY"));
	OwnerCharacter->AttachActorAtSocket(HolsteredWeaponSocketName, this);
	PlayEquipWeaponSound();
	
	PlayerOwnerController = PlayerOwnerController == nullptr ?
		Cast<ABasePlayerController>(OwnerCharacter->Controller) : PlayerOwnerController;
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


void AWeaponActor::GetCrosshair(float DeltaTime, bool bIsAiming, TObjectPtr<UTexture2D>& Center, TObjectPtr<UTexture2D>& Left,
                                TObjectPtr<UTexture2D>& Right, TObjectPtr<UTexture2D>& Top, TObjectPtr<UTexture2D>& Bottom, float &Spread)
{
	Center = nullptr;
	Left = nullptr;
	Right = nullptr;
	Top = nullptr;
	Bottom = nullptr;
	Spread = 0.5f;
}

void AWeaponActor::Fire(const TArray<FVector_NetQuantize>& HitTargets)
{
}

bool AWeaponActor::CanReload()
{
	return true;
}

TArray<FVector_NetQuantize> AWeaponActor::GetTarget(FVector& HitTarget)
{
	return Targeting->GetTarget(HitTarget);
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