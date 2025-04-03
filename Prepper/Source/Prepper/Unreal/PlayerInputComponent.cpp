// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerInputComponent.h"
#include "EnhancedInputComponent.h"
#include "Prepper/GamePlay/CharacterController/CharacterController.h"
#include "Prepper/GamePlay/CharacterController/Combat.h"


class UEnhancedInputLocalPlayerSubsystem;
// Sets default values for this component's properties
UPlayerInputComponent::UPlayerInputComponent()
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

void UPlayerInputComponent::Move(const FInputActionValue& Value)
{
	if (CC == nullptr) return;
	CC->Move(Value);
}

void UPlayerInputComponent::Look(const FInputActionValue& Value)
{
	if (CC == nullptr) return;
	CC->Look(Value);
}

void UPlayerInputComponent::JumpButtonPressed()
{
	if (CC == nullptr) return;
	CC->JumpTrigger(true);
}

void UPlayerInputComponent::JumpButtonReleased()
{
	if (CC == nullptr) return;
	CC->JumpTrigger(false);
}

void UPlayerInputComponent::CrouchButtonPressed()
{
	if (CC == nullptr) return;
	CC->CrouchToggle();
}

void UPlayerInputComponent::ReloadButtonPressed()
{
	if (CC == nullptr) return;
	CC->GetCombat()->Reload();
}

void UPlayerInputComponent::AimButtonPressed()
{
	if (CC == nullptr) return;
	CC->GetCombat()->AimTrigger(true);
}

void UPlayerInputComponent::AimButtonReleased()
{
	if (CC == nullptr) return;
	CC->GetCombat()->AimTrigger(false);
}

void UPlayerInputComponent::FireButtonPressed()
{
	if (CC == nullptr) return;
	CC->GetCombat()->AttackTrigger(true);
}

void UPlayerInputComponent::FireButtonReleased()
{
	if (CC == nullptr) return;
	CC->GetCombat()->AttackTrigger(false);
}

void UPlayerInputComponent::SprintButtonPressed()
{
	if (CC == nullptr) return;
	CC->SprintTrigger(true);
}

void UPlayerInputComponent::SprintButtonReleased()
{
	if (CC == nullptr) return;
	CC->SprintTrigger(false);
}

void UPlayerInputComponent::EquipButtonPressed()
{
	if (CC == nullptr) return;
	CC->EquipButtonPressed();
}

TObjectPtr<UInputMappingContext> UPlayerInputComponent::GetMappingContext()
{
	return PlayerMappingContext;
}

void UPlayerInputComponent::SetInput(ICharacterController* cc, UEnhancedInputComponent* Input)
{
	CC = cc;

	// Moving
	Input->BindAction(MoveAction, ETriggerEvent::Triggered,
	                  this, &UPlayerInputComponent::Move);
	Input->BindAction(MoveAction, ETriggerEvent::Completed,
	                  this, &UPlayerInputComponent::Move);

	// Looking
	Input->BindAction(LookAction, ETriggerEvent::Triggered,
	                  this, &UPlayerInputComponent::Look);

	// Jumping
	Input->BindAction(JumpAction, ETriggerEvent::Started,
	                  this, &UPlayerInputComponent::JumpButtonPressed);
	Input->BindAction(JumpAction, ETriggerEvent::Completed,
	                  this, &UPlayerInputComponent::JumpButtonReleased);

	//Sprint
	Input->BindAction(SprintAction, ETriggerEvent::Triggered,
	                  this, &UPlayerInputComponent::SprintButtonPressed);
	Input->BindAction(SprintAction, ETriggerEvent::Completed, this,
	                  &UPlayerInputComponent::SprintButtonReleased);

	// Equip
	Input->BindAction(EquipAction, ETriggerEvent::Triggered, this,
	                  &UPlayerInputComponent::EquipButtonPressed);

	// Crouch
	Input->BindAction(CrouchAction, ETriggerEvent::Triggered, this,
	                  &UPlayerInputComponent::CrouchButtonPressed);

	//Aim
	Input->BindAction(AimAction, ETriggerEvent::Triggered, this,
	                  &UPlayerInputComponent::AimButtonPressed);
	Input->BindAction(AimAction, ETriggerEvent::Completed, this,
	                  &UPlayerInputComponent::AimButtonReleased);

	// Fire
	Input->BindAction(FireAction, ETriggerEvent::Triggered, this,
	                  &UPlayerInputComponent::FireButtonPressed);
	Input->BindAction(FireAction, ETriggerEvent::Completed, this,
	                  &UPlayerInputComponent::FireButtonReleased);

	// Reload
	Input->BindAction(ReloadAction, ETriggerEvent::Triggered, this,
	                  &UPlayerInputComponent::ReloadButtonPressed);

}