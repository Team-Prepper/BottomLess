#include "CharacterControlMapper.h"

#include "Blueprint/UserWidget.h"
#include "Prepper/___Legacy/Character/BaseCharacter.h"
#include "Prepper/___Legacy/Character/PlayerCharacter.h"
#include "Prepper/___Legacy/Character/Component/Combat/BaseCombatComponent.h"
#include "Prepper/___Legacy/HUD/UI/CharacterOverlay/WeaponWidget.h"

void UCharacterControlMapper::Connect(APlayerController* TargetController)
{
	if (!WeaponOverlay)
	{
		if (!WeaponOverlayClass) return;
		
		WeaponOverlay = CreateWidget<UWeaponWidget>(TargetController, WeaponOverlayClass);
	}
	
	if (!WeaponOverlay) return;

	WeaponOverlay->AddToViewport();

	TargetCharacter->GetCombatComponent()->Attach(static_cast<IObserver<GaugeValue<int>>*>(WeaponOverlay));
	TargetCharacter->GetCombatComponent()->Attach(static_cast<IObserver<FString>*>(WeaponOverlay));

}

void UCharacterControlMapper::Disconnect()
{
	if (!WeaponOverlay) return;
	
	TargetCharacter->GetCombatComponent()->Detach(static_cast<IObserver<GaugeValue<int>>*>(WeaponOverlay));
	TargetCharacter->GetCombatComponent()->Detach(static_cast<IObserver<FString>*>(WeaponOverlay));
	WeaponOverlay->RemoveFromParent();
}

void UCharacterControlMapper::Move(const FInputActionValue& Value)
{
	TargetCharacter->Move(Value);
}

void UCharacterControlMapper::Look(const FInputActionValue& Value)
{
	TargetCharacter->Look(Value);
}

void UCharacterControlMapper::ShiftPressed()
{
	TargetCharacter->SetMovementState(EMovementState::EMS_Sprint);
}

void UCharacterControlMapper::ShiftReleased()
{
	TargetCharacter->SetMovementState(EMovementState::EMS_Idle);
}

void UCharacterControlMapper::SpacePressed()
{
	TargetCharacter->Jump();
}

void UCharacterControlMapper::SpaceReleased()
{
	TargetCharacter->StopJumping();
}

void UCharacterControlMapper::EPressed()
{
	TargetCharacter->Interaction();
}

void UCharacterControlMapper::RPressed()
{
	TargetCharacter->Reload();
}

void UCharacterControlMapper::ControlPressed()
{
	TargetCharacter->CrouchToggle();
}

void UCharacterControlMapper::MouseLeftPressed()
{
	TargetCharacter->AttackTrigger(true);
}

void UCharacterControlMapper::MouseLeftReleased()
{
	TargetCharacter->AttackTrigger(false);
}

void UCharacterControlMapper::MouseRightPressed()
{
	TargetCharacter->AimTrigger(true);
}

void UCharacterControlMapper::MouseRightReleased()
{
	TargetCharacter->AimTrigger(false);
}

UCameraComponent* UCharacterControlMapper::GetFollowCamera()
{
	return TargetCharacter->GetFollowCamera();
}
