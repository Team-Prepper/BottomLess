// Fill out your copyright notice in the Description page of Project Settings.


#include "BCharacter.h"

#include "CharacterAnimInstance.h"
#include "InputActionValue.h"
#include "Component/CustomCameraComponent.h"
#include "Component/ElimDissolveComponent.h"
#include "Component/InteractionHandlingComponent.h"
#include "Component/FlexibleSpringArmComponent/FlexibleSpringArmComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "Prepper/GamePlay/PrepperGameMode.h"
#include "Prepper/GamePlay/Car/Car.h"
#include "Prepper/GamePlay/Character/Component/AmmoBoxComponent.h"
#include "Prepper/GamePlay/Item/Object/ItemBackpack.h"
#include "Prepper/GamePlay/Weapon/Weapon.h"
#include "Prepper/GamePlay/Weapon/WeaponTypes.h"
#include "Prepper/Unreal/CharacterComponent/UnrealCharacterMoveComponent.h"
#include "Prepper/Unreal/CharacterComponent/UnrealCombatComponent.h"
#include "Prepper/Unreal/CharacterComponent/UnrealStatusComponent.h"
#include "Prepper/Unreal/Inventory/UnrealInventoryComponent.h"
#include "Prepper/___Legacy/Car/LegacyCarPawn.h"

// Sets default values
ABCharacter::ABCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
	FlexibleCameraBoom = CreateDefaultSubobject<UFlexibleSpringArmComponent>(TEXT("FlexibleCameraBoom"));
	FlexibleCameraBoom->ChangeArmOffsetToTemplate(FString("Default"));
	FlexibleCameraBoom->SetupAttachment(GetMesh());
	FlexibleCameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCustomCameraComponent>(TEXT("FollowCam"));
	FollowCamera->SetupAttachment(FlexibleCameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
	PawnSensing->SetComponentTickEnabled(false);

	Status = CreateDefaultSubobject<UUnrealStatusComponent>(TEXT("StatusComponent"));
	Combat = CreateDefaultSubobject<UUnrealCombatComponent>(TEXT("CombatComponent"));
	Inventory = CreateDefaultSubobject<UUnrealInventoryComponent>(TEXT("Inventory"));
	
	Interaction = CreateDefaultSubobject<UInteractionHandlingComponent>(TEXT("InteractionComponent"));
	CharacterMove = CreateDefaultSubobject<UUnrealCharacterMoveComponent>(TEXT("CharacterMoveComponent"));
	ElimDissolve = CreateDefaultSubobject<UElimDissolveComponent>(TEXT("ElimDessolveComponent"));

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
}

TObjectPtr<APawn> ABCharacter::GetPawn()
{
	return this;
}

TObjectPtr<UStatusComponent> ABCharacter::GetStatus()
{
	return Status;
}

TObjectPtr<UBCombatComponent> ABCharacter::GetCombat()
{
	return Combat;
}

TObjectPtr<UInventoryComponent> ABCharacter::GetInventory()
{
	return Inventory;
}

TObjectPtr<UCharacterMoveComponent> ABCharacter::GetMove()
{
	return CharacterMove;
}

IAmmoBox* ABCharacter::GetAmmoBox()
{
	return AmmoBox;
}

void ABCharacter::SetAmmoBox(const TObjectPtr<UAmmoBoxComponent> NewAmmoBox)
{
	AmmoBox = NewAmmoBox;
}

void ABCharacter::Boarding(TObjectPtr<ACar> Vehicle)
{
	CharacterMove->SetCar(Vehicle);
}

void ABCharacter::GetOff()
{
	CharacterMove->SetCar(nullptr);
	//GetController<APlayerController>()->SetViewTarget(this);
}

void ABCharacter::EquipBackpack(TObjectPtr<AItemBackpack> Backpack)
{
	AttachActorAtSocket(FName("BackpackSocket"), Backpack);
	Inventory->ChangingInventory(Backpack->GetInventory());
}

void ABCharacter::PlayAnim(UAnimMontage* Montage, const FName& SectionName) const
{
	if (Montage == nullptr) return;
	UE_LOG(LogTemp, Warning, TEXT("Play Anim"));
	
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	
	if (!AnimInstance) return;
	
	AnimInstance->Montage_Play(Montage);
	
	if (SectionName.IsEqual("")) return;
	
	AnimInstance->Montage_JumpToSection(SectionName);
}

void ABCharacter::GetLookDirection(FVector& Start, FVector& Forward) const
{
	Start = FollowCamera->GetComponentLocation();
	Forward = FollowCamera->GetForwardVector();
}

void ABCharacter::AttachActorAtSocket(FName SocketName, AActor* TargetActor) const
{
	if(const USkeletalMeshSocket* TargetSocket
		= GetMesh()->GetSocketByName(SocketName))
	{
		//AttachedActor.Add(TargetActor);
		TargetSocket->AttachActor(TargetActor, GetMesh());
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Attach %s"), *SocketName.ToString());
}

void ABCharacter::SetEquippedWeaponType(const EWeaponType WeaponType)
{
	bUseControllerRotationYaw = WeaponType != EWeaponType::EWT_MAX;
	GetCharacterMovement()->bOrientRotationToMovement = !bUseControllerRotationYaw;
	Cast<UCharacterAnimInstance>(GetMesh()->GetAnimInstance())->SetEquippedWeaponType(WeaponType);
}

void ABCharacter::AddItem(const FString& ItemCode, int Count)
{

}

void ABCharacter::UseQuickSlotItem(int Idx)
{

}

void ABCharacter::EquipWeapon(AWeapon* Weapon)
{

}

void ABCharacter::EquipBackpack(AItemBackpack* BackpackToEquip)
{

}

void ABCharacter::Heal(float Amount)
{
	GetStatus()->AddHP(Amount);
}

void ABCharacter::Eat(float Amount)
{
	GetStatus()->AddHP(Amount);
}

void ABCharacter::Drink(float Amount)
{
	GetStatus()->AddHP(Amount);
}

UInventoryComponent* ABCharacter::GetInventory() const
{
	return Inventory;
}

void ABCharacter::ReceiveDamage(float Damage, AController* InstigatorController, AActor* DamageCauser)
{
	GetStatus()->TakeDamage(Damage);
	
	if (GetStatus()->GetCurHealth() > 0) return;

	APrepperGameMode* PrepperGameMode =  GetWorld()->GetAuthGameMode<APrepperGameMode>();
	
	if(PrepperGameMode == nullptr) return;
	
	ABasePlayerController* PrepperPlayerController = GetController<ABasePlayerController>();
	ABasePlayerController* AttackerController = Cast<ABasePlayerController>(InstigatorController);
	
	PrepperGameMode->PlayerEliminated(this, PrepperPlayerController, AttackerController);
	
}

void ABCharacter::ElimCharacter() const
{
	//PlayAnim(ElimMontage);
	ElimDissolve->TargetElim();
	
}

void ABCharacter::SetTeamIdx(int Idx)
{
	TeamIdx = Idx;
}

TObjectPtr<UPawnSensingComponent> ABCharacter::GetPawnSensing() const
{
	return PawnSensing;
}

TArray<FString> ABCharacter::GetEquipmentCodes() const
{
	TArray<FString> Retval;

	if (Combat->GetEquippedWeapon() != nullptr)
	{
		Retval.Add(Combat->GetEquippedWeapon()->GetCode());
	}
	if (Combat->GetSecondaryWeapon() != nullptr)
	{
		Retval.Add(Combat->GetSecondaryWeapon()->GetCode());
	}
	/*
	if (EquippedBackpack != nullptr)
	{
		Retval.Add(EquippedBackpack->GetCode());
	}*/

	return Retval;
}

void ABCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	Combat->SetTargetCharacter(this);
	Interaction->SetTargetCharacter(this);
	Inventory->SetOwner(this);
	ElimDissolve->SetTargetCharacter(this);
}

void ABCharacter::SpawnWeaponActor()
{
	if(!HasAuthority()) return;
	if (WeaponActorClass == nullptr) return;
	
	UWorld* World = GetWorld();
	if (World == nullptr) return;

	const FVector Location = GetActorLocation();
	const FRotator Rotation = GetActorRotation();
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this; // Setting the owner

	const TObjectPtr<AWeapon> EquippedWeapon =
		World->SpawnActor<AWeapon>(WeaponActorClass, Location, Rotation, SpawnParams);

	if (!EquippedWeapon) return;

	EquippedWeapon->Interaction(this);
}

void ABCharacter::Move(const FInputActionValue& Value)
{
	if (Controller == nullptr) return;
	
	if (CharacterMove->GetTargetCar() != nullptr) return;
	
	const FVector2D MovementVector = Value.Get<FVector2D>();
	
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(RightDirection, MovementVector.X);
	AddMovementInput(ForwardDirection, MovementVector.Y);
}

void ABCharacter::Look(const FInputActionValue& Value)
{
	if (Controller == nullptr) return;
	
	if (CharacterMove->GetTargetCar() != nullptr)
	{
		CharacterMove->GetTargetCar()->Look(Value);
		return;
	}
	
	const FVector2D LookAxisVector = Value.Get<FVector2D>();
	
	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
	
}

void ABCharacter::CrouchToggle()
{
	if (CharacterMove->GetTargetCar() != nullptr)
	{
		CharacterMove->GetTargetCar()->ChangeCam();
		return;
	}
	CharacterMove->CrouchToggle();
}

void ABCharacter::JumpTrigger(bool IsTrigger)
{
	if (CharacterMove->GetTargetCar() != nullptr) return;
	CharacterMove->JumpTrigger(IsTrigger);
}

void ABCharacter::Crouch(bool bClientSimulation)
{
	Super::Crouch(bClientSimulation);
	
	Cast<UCharacterAnimInstance>(GetMesh()->GetAnimInstance())->SetCrouch(true);
	FlexibleCameraBoom->ChangeArmOffsetToTemplate(FString("Crouch"));
}


void ABCharacter::UnCrouch(bool bClientSimulation)
{
	Super::UnCrouch(bClientSimulation);
	
	Cast<UCharacterAnimInstance>(GetMesh()->GetAnimInstance())->SetCrouch(false);
	FlexibleCameraBoom->ChangeArmOffsetToTemplate(FString("Default"));
}

void ABCharacter::SprintTrigger(bool IsTrigger)
{
	if (CharacterMove->GetTargetCar() != nullptr) return;
	CharacterMove->SprintTrigger(IsTrigger);
}

void ABCharacter::EquipButtonPressed()
{
	if (CharacterMove->GetTargetCar() != nullptr)
	{
		CharacterMove->GetTargetCar()->InteractionAct(this);
		return;
	}
	Interaction->Interaction();
}

void ABCharacter::SetMaxSpeed(const float MaxSpeed) const
{
	GetCharacterMovement()->MaxWalkSpeed = MaxSpeed;
}

void ABCharacter::AimTrigger(const bool IsTrigger) const
{
	Cast<UCharacterAnimInstance>(GetMesh()->GetAnimInstance())->SetAiming(IsTrigger);
	CharacterMove->SetAiming(IsTrigger);
}

void ABCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}