// Fill out your copyright notice in the Description page of Project Settings.


#include "UnrealPlayerInputComponent.h"
#include "EnhancedInputComponent.h"
#include "UnrealPlayerController.h"
#include "Prepper/GamePlay/Character/BCharacter.h"
#include "Prepper/GamePlay/Character/Component/BCombatComponent.h"


class UEnhancedInputLocalPlayerSubsystem;
// Sets default values for this component's properties
UUnrealPlayerInputComponent::UUnrealPlayerInputComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
	CC = nullptr;
	
	PlayerMappingContext = nullptr;
	
	MoveAction = nullptr;
	JumpAction = nullptr;
	LookAction = nullptr;
	SprintAction = nullptr;
	EquipAction = nullptr;
	CrouchAction = nullptr;
	ReloadAction = nullptr;
	AimAction = nullptr;
	FireAction = nullptr;
	OpenInventory = nullptr;
	OpenSetting = nullptr;

}

void UUnrealPlayerInputComponent::Move(const FInputActionValue& Value)
{
	if (CC == nullptr) return;
	CC->GetTargetCharacter()->Move(Value);
}

void UUnrealPlayerInputComponent::Look(const FInputActionValue& Value)
{
	if (CC == nullptr) return;
	CC->GetTargetCharacter()->Look(Value);
}

void UUnrealPlayerInputComponent::JumpButtonPressed()
{
	if (CC == nullptr) return;
	CC->GetTargetCharacter()->JumpTrigger(true);
}

void UUnrealPlayerInputComponent::JumpButtonReleased()
{
	if (CC == nullptr) return;
	CC->GetTargetCharacter()->JumpTrigger(false);
}

void UUnrealPlayerInputComponent::CrouchButtonPressed()
{
	if (CC == nullptr) return;
	CC->GetTargetCharacter()->CrouchToggle();
}

void UUnrealPlayerInputComponent::ReloadButtonPressed()
{
	if (CC == nullptr) return;
	CC->GetTargetCharacter()->GetCombat()->Reload();
}

void UUnrealPlayerInputComponent::AimButtonPressed()
{
	if (CC == nullptr) return;
	CC->GetTargetCharacter()->GetCombat()->AimTrigger(true);
}

void UUnrealPlayerInputComponent::AimButtonReleased()
{
	if (CC == nullptr) return;
	CC->GetTargetCharacter()->GetCombat()->AimTrigger(false);
}

void UUnrealPlayerInputComponent::AttackButtonPressed()
{
	if (CC == nullptr) return;
	CC->GetTargetCharacter()->GetCombat()->AttackTrigger(true);
}

void UUnrealPlayerInputComponent::AttackButtonReleased()
{
	if (CC == nullptr) return;
	CC->GetTargetCharacter()->GetCombat()->AttackTrigger(false);
}

void UUnrealPlayerInputComponent::TabButtonPressed()
{
	if (CC == nullptr) return;
	CC->TabButtonPressed();
}

void UUnrealPlayerInputComponent::TabButtonReleased()
{
	if (CC == nullptr) return;
	CC->TabButtonReleased();
}

void UUnrealPlayerInputComponent::SprintButtonPressed()
{
	if (CC == nullptr) return;
	CC->GetTargetCharacter()->SprintTrigger(true);
}

void UUnrealPlayerInputComponent::SprintButtonReleased()
{
	if (CC == nullptr) return;
	CC->GetTargetCharacter()->SprintTrigger(false);
}

void UUnrealPlayerInputComponent::EquipButtonPressed()
{
	if (CC == nullptr) return;
	CC->GetTargetCharacter()->EquipButtonPressed();
}

TObjectPtr<UInputMappingContext> UUnrealPlayerInputComponent::GetMappingContext()
{
	return PlayerMappingContext;
}

void UUnrealPlayerInputComponent::SetInput(TObjectPtr<AUnrealPlayerController> cc, UEnhancedInputComponent* Input)
{
	CC = cc;

	// Moving
	Input->BindAction(MoveAction, ETriggerEvent::Triggered,
	                  this, &UUnrealPlayerInputComponent::Move);
	Input->BindAction(MoveAction, ETriggerEvent::Completed,
	                  this, &UUnrealPlayerInputComponent::Move);

	// Looking
	Input->BindAction(LookAction, ETriggerEvent::Triggered,
	                  this, &UUnrealPlayerInputComponent::Look);

	// Jumping
	Input->BindAction(JumpAction, ETriggerEvent::Started,
	                  this, &UUnrealPlayerInputComponent::JumpButtonPressed);
	Input->BindAction(JumpAction, ETriggerEvent::Completed,
	                  this, &UUnrealPlayerInputComponent::JumpButtonReleased);

	//Sprint
	Input->BindAction(SprintAction, ETriggerEvent::Started,
	                  this, &UUnrealPlayerInputComponent::SprintButtonPressed);
	Input->BindAction(SprintAction, ETriggerEvent::Completed, this,
	                  &UUnrealPlayerInputComponent::SprintButtonReleased);

	// Equip
	Input->BindAction(EquipAction, ETriggerEvent::Started, this,
	                  &UUnrealPlayerInputComponent::EquipButtonPressed);

	// Crouch
	Input->BindAction(CrouchAction, ETriggerEvent::Started, this,
	                  &UUnrealPlayerInputComponent::CrouchButtonPressed);

	//Aim
	Input->BindAction(AimAction, ETriggerEvent::Started, this,
	                  &UUnrealPlayerInputComponent::AimButtonPressed);
	Input->BindAction(AimAction, ETriggerEvent::Completed, this,
	                  &UUnrealPlayerInputComponent::AimButtonReleased);

	// Fire
	Input->BindAction(FireAction, ETriggerEvent::Started, this,
	                  &UUnrealPlayerInputComponent::AttackButtonPressed);
	Input->BindAction(FireAction, ETriggerEvent::Completed, this,
	                  &UUnrealPlayerInputComponent::AttackButtonReleased);

	// Reload
	Input->BindAction(ReloadAction, ETriggerEvent::Triggered, this,
	                  &UUnrealPlayerInputComponent::ReloadButtonPressed);

	Input->BindAction(OpenInventory, ETriggerEvent::Started, this, &UUnrealPlayerInputComponent::TabButtonPressed);
	Input->BindAction(OpenInventory, ETriggerEvent::Completed, this, &UUnrealPlayerInputComponent::TabButtonReleased);

}