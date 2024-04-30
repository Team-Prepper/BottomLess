#include "Weapon.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "Prepper/Prepper.h"
#include "Prepper/Character/PlayerCharacter.h"
#include "Prepper/PlayerController/PrepperPlayerController.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicateMovement(true);
	
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);

	WeaponMesh->SetCollisionResponseToAllChannels(ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	StaticWeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeleeWeaponMesh"));
	StaticWeaponMesh->SetupAttachment(RootComponent);

	WeaponMesh->SetCustomDepthStencilValue(CustomDepthColor);
	StaticWeaponMesh->SetCustomDepthStencilValue(CustomDepthColor);
	WeaponMesh->MarkRenderStateDirty();
	StaticWeaponMesh->MarkRenderStateDirty();
	EnableCustomDepth(true);
	
	AreaSphere = CreateDefaultSubobject<USphereComponent>("AreaSphere");
	AreaSphere->SetupAttachment(RootComponent);
	AreaSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	PickUpWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickUpWidget"));
	PickUpWidget->SetupAttachment(RootComponent);
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	CustomDepthColor = CUSTOM_DEPTH_MINT;

	WeaponMesh->SetCustomDepthStencilValue(CustomDepthColor);
	StaticWeaponMesh->SetCustomDepthStencilValue(CustomDepthColor);
	WeaponMesh->MarkRenderStateDirty();
	StaticWeaponMesh->MarkRenderStateDirty();
	EnableCustomDepth(true);
	
	if(HasAuthority())
	{
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        AreaSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
        AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnSphereOverlap);
        AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnSphereEndOverlap);
	}
}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeapon, WeaponState);
}

void AWeapon::EnableCustomDepth(bool bEnable)
{
	if(WeaponMesh)
	{
		WeaponMesh->SetRenderCustomDepth(bEnable);
	}
	if(StaticWeaponMesh)
	{
		StaticWeaponMesh->SetRenderCustomDepth(bEnable);
	}
}

void AWeapon::OnRep_Owner()
{
	Super::OnRep_Owner();
	if(Owner == nullptr)
	{
		PlayerOwnerCharacter = nullptr;
		PlayerOwnerController = nullptr;
	}
}

void AWeapon::Dropped()
{
	SetWeaponState(EWeaponState::EWS_Dropped);
	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
	WeaponMesh->DetachFromComponent(DetachRules);
	StaticWeaponMesh->DetachFromComponent(DetachRules);
	SetOwner(nullptr);
	PlayerOwnerCharacter = nullptr;
	PlayerOwnerController = nullptr;
}

void AWeapon::Interaction(APlayerCharacter* Target)
{
	Target->EquipWeapon(this);
}

void AWeapon::SetWeaponState(EWeaponState State)
{
	WeaponState = State;
	OnWeaponStateSet();
}

// client
void AWeapon::OnRep_WeaponState()
{
	OnWeaponStateSet();
}

void AWeapon::OnWeaponStateSet()
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
	}
}

void AWeapon::OnEquipped()
{
	ShowPickUpWidget(false);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetSimulatePhysics(false);
	WeaponMesh->SetEnableGravity(false);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StaticWeaponMesh->SetSimulatePhysics(false);
	StaticWeaponMesh->SetEnableGravity(false);
	StaticWeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	EnableCustomDepth(false);

	PlayerOwnerCharacter = PlayerOwnerCharacter == nullptr ? Cast<APlayerCharacter>(GetOwner()) : PlayerOwnerCharacter;
	if (PlayerOwnerCharacter && bUseServerSideRewind)
	{
		PlayerOwnerController = PlayerOwnerController == nullptr ? Cast<APrepperPlayerController>(PlayerOwnerCharacter->Controller) : PlayerOwnerController;
		if (PlayerOwnerController && HasAuthority() && !PlayerOwnerController->HighPingDelegate.IsBound())
		{
			PlayerOwnerController->HighPingDelegate.AddDynamic(this, &AWeapon::OnPingTooHigh);
		}
	}
}

void AWeapon::OnDropped()
{
	if (HasAuthority())
	{
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
	WeaponMesh->SetSimulatePhysics(true);
	WeaponMesh->SetEnableGravity(true);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	StaticWeaponMesh->SetSimulatePhysics(false);
	StaticWeaponMesh->SetEnableGravity(false);
	StaticWeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	WeaponMesh->SetCustomDepthStencilValue(CustomDepthColor);
	WeaponMesh->MarkRenderStateDirty();
	EnableCustomDepth(true);

	PlayerOwnerCharacter = PlayerOwnerCharacter == nullptr ? Cast<APlayerCharacter>(GetOwner()) : PlayerOwnerCharacter;
	if (PlayerOwnerCharacter)
	{
		PlayerOwnerController = PlayerOwnerController == nullptr ? Cast<APrepperPlayerController>(PlayerOwnerCharacter->Controller) : PlayerOwnerController;
		if (PlayerOwnerController && HasAuthority() && PlayerOwnerController->HighPingDelegate.IsBound())
		{
			PlayerOwnerController->HighPingDelegate.RemoveDynamic(this, &AWeapon::OnPingTooHigh);
		}
	}
}

void AWeapon::OnEquippedSecondary()
{
	ShowPickUpWidget(false);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetSimulatePhysics(false);
	WeaponMesh->SetEnableGravity(false);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StaticWeaponMesh->SetSimulatePhysics(false);
	StaticWeaponMesh->SetEnableGravity(false);
	StaticWeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	/* custom Depth 아이템 테두리 윤곽선 등에 있는 장비에 넣을것인가?
	if (WeaponMesh)
	{
		WeaponMesh->SetCustomDepthStencilValue(CustomDepthColor);
		WeaponMesh->MarkRenderStateDirty();
	}
	*/
	PlayerOwnerCharacter = PlayerOwnerCharacter == nullptr ? Cast<APlayerCharacter>(GetOwner()) : PlayerOwnerCharacter;
	if (PlayerOwnerCharacter)
	{
		PlayerOwnerController = PlayerOwnerController == nullptr ? Cast<APrepperPlayerController>(PlayerOwnerCharacter->Controller) : PlayerOwnerController;
		if (PlayerOwnerController && HasAuthority() && PlayerOwnerController->HighPingDelegate.IsBound())
		{
			PlayerOwnerController->HighPingDelegate.RemoveDynamic(this, &AWeapon::OnPingTooHigh);
		}
	}
}

void AWeapon::OnPingTooHigh(bool bPingTooHigh)
{
	bUseServerSideRewind = !bPingTooHigh;
}
