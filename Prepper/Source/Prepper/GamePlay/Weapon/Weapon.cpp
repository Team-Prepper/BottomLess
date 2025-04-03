#include "Weapon.h"

#include "Components/PawnNoiseEmitterComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Prepper/GamePlay/CharacterController/Combat.h"
#include "Prepper/__Legacy/Weapon/AimingEffect/PlayerAimingEffect.h"
#include "Sound/SoundCue.h"
#include "Targeting/WeaponTargeting.h"

AWeapon::AWeapon()
{
	AActor::SetReplicateMovement(true);
	
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);

	WeaponMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	WeaponMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	StaticWeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeleeWeaponMesh"));
	StaticWeaponMesh->SetupAttachment(RootComponent);
	StaticWeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	WeaponMesh->MarkRenderStateDirty();
	StaticWeaponMesh->MarkRenderStateDirty();
	StaticWeaponMesh->SetSimulatePhysics(false);
	StaticWeaponMesh->SetEnableGravity(false);

	Targeting = CreateDefaultSubobject<UWeaponTargeting>("WeaponTargeting");

	// 노이즈 생성 컴포넌트 추가
	PawnNoiseEmitter = CreateDefaultSubobject<UPawnNoiseEmitterComponent>(TEXT("PawnNoiseEmitter"));
	
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

void AWeapon::PlayEquipWeaponSound()
{
	if (!GetOwner()) return;
	if (!EquipSound) return;
	
	UGameplayStatics::PlaySoundAtLocation(
		GetOwner(), EquipSound,
		GetOwner()->GetActorLocation()
	);
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
}

void AWeapon::Interaction(ICharacterController* Target)
{
	Target->GetCombat()->EquipWeapon(this);
}

void AWeapon::OnWeaponStateSet()
{
}

void AWeapon::OnEquipped()
{
	WeaponPhysicsActive(false);
	PlayEquipWeaponSound();
}

void AWeapon::OnDropped()
{
	UE_LOG(LogTemp, Warning , TEXT("WEAPON : WEAPON DROPPED"));
	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);

	WeaponPhysicsActive(true);
	WeaponMesh->DetachFromComponent(DetachRules);
	
	SetOwner(nullptr);
	
}

void AWeapon::OnEquippedSecondary()
{
	WeaponPhysicsActive(false);
	PlayEquipWeaponSound();
}

void AWeapon::GetCrosshair(float DeltaTime, bool bIsAiming, TObjectPtr<UTexture2D>& Center, TObjectPtr<UTexture2D>& Left,
                                TObjectPtr<UTexture2D>& Right, TObjectPtr<UTexture2D>& Top, TObjectPtr<UTexture2D>& Bottom, float &Spread)
{
	Center = nullptr;
	Left = nullptr;
	Right = nullptr;
	Top = nullptr;
	Bottom = nullptr;
	Spread = 0.5f;
}

void AWeapon::Fire(const TArray<FVector_NetQuantize>& HitTargets)
{
}

bool AWeapon::CanReload()
{
	return true;
}

TArray<FVector_NetQuantize> AWeapon::GetTarget(FVector& HitTarget) const
{
	return Targeting->GetTarget(HitTarget);
}

void AWeapon::WeaponPhysicsActive(bool bActive)
{
	SetActorEnableCollision(bActive);
	
	WeaponMesh->SetSimulatePhysics(bActive);
	WeaponMesh->SetEnableGravity(bActive);

	if (!bActive)
	{
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		return;
	}
	
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

}