// Fill out your copyright notice in the Description page of Project Settings.


#include "BCharacter.h"

#include "CharacterAnimInstance.h"
#include "InputActionValue.h"
#include "Component/CustomCameraComponent.h"
#include "Component/FlexibleSpringArmComponent/FlexibleSpringArmComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "Prepper/GamePlay/Character/Component/AmmoBoxComponent.h"
#include "Prepper/GamePlay/Weapon/WeaponTypes.h"
#include "Prepper/Unreal/Component/UnrealCharacterMoveComponent.h"
#include "Prepper/Unreal/Component/UnrealCombatComponent.h"
#include "Prepper/Unreal/Component/UnrealInteractionComponent.h"
#include "Prepper/Unreal/Component/UnrealStatusComponent.h"
#include "Prepper/Unreal/Inventory/UnrealInventoryComponent.h"

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
	
	Interaction = CreateDefaultSubobject<UUnrealInteractionComponent>(TEXT("InteractionComponent"));
	CharacterMove = CreateDefaultSubobject<UUnrealCharacterMoveComponent>(TEXT("CharacterMoveComponent"));
	
	bUseControllerRotationYaw = false;
}

TObjectPtr<UStatusComponent> ABCharacter::GetStatus()
{
	return Status;
}

TObjectPtr<UBCombatComponent> ABCharacter::GetCombat()
{
	return Combat;
}

IAmmoBox* ABCharacter::GetAmmoBox()
{
	return AmmoBox;
}

void ABCharacter::SetAmmoBox(const TObjectPtr<UAmmoBoxComponent> NewAmmoBox)
{
	AmmoBox = NewAmmoBox;
}

void ABCharacter::PlayAnim(const FString& String)
{
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

}

void ABCharacter::Eat(float Amount)
{

}

void ABCharacter::Drink(float Amount)
{

}

UInventoryComponent* ABCharacter::GetInventory() const
{
	return Inventory;
}

void ABCharacter::ReceiveDamage(float Damage, AController* InstigatorController, AActor* DamageCauser)
{
	GetStatus()->TakeDamage(Damage);
	
	if (GetStatus()->GetCurHealth() > 0) return;

	/*
	// 해당 캐릭터가 사망했다면 
	APrepperGameMode* PrepperGameMode =  GetWorld()->GetAuthGameMode<APrepperGameMode>();
	
	if(PrepperGameMode == nullptr) return;
	
	ABasePlayerController* PrepperPlayerController = Cast<ABasePlayerController>(Controller);
	ABasePlayerController* AttackerController = Cast<ABasePlayerController>(InstigatorController);
	
	PrepperGameMode->PlayerEliminated(this, PrepperPlayerController, AttackerController);*/

}

void ABCharacter::SetTeamIdx(int Idx)
{
	TeamIdx = Idx;
}

TObjectPtr<UPawnSensingComponent> ABCharacter::GetPawnSensing() const
{
	return PawnSensing;
}

void ABCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	Combat->SetTargetCharacter(this);
	Interaction->SetTargetCharacter(this);
	Inventory->SetOwner(this);
}

void ABCharacter::Move(const FInputActionValue& Value)
{
	if (Controller == nullptr) return;
	
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
	
	const FVector2D LookAxisVector = Value.Get<FVector2D>();
	
	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
	
}

void ABCharacter::CrouchToggle()
{
	CharacterMove->CrouchToggle();
}

void ABCharacter::JumpTrigger(bool IsTrigger)
{
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
	CharacterMove->SprintTrigger(IsTrigger);
}

void ABCharacter::EquipButtonPressed()
{
	Interaction->Interaction();
}

void ABCharacter::SetMaxSpeed(float MaxSpeed)
{
	GetCharacterMovement()->MaxWalkSpeed = MaxSpeed;
}

void ABCharacter::AimTrigger(bool IsTrigger)
{
	Cast<UCharacterAnimInstance>(GetMesh()->GetAnimInstance())->SetAiming(IsTrigger);
	CharacterMove->SetAiming(IsTrigger);
}

void ABCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}