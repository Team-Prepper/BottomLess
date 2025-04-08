#include "CarControlMapper.h"

#include "Blueprint/UserWidget.h"
#include "Prepper/__Legacy/Car/CarPawn.h"

void UCarControlMapper::Connect(APlayerController* TargetController)
{
	if (!CarOverlay)
	{
		if (!CarOverlayClass) return;
		
		CarOverlay = CreateWidget<UUserWidget>(TargetController, CarOverlayClass);
	}
	
	if (!CarOverlay) return;

	CarOverlay->AddToViewport();
}

void UCarControlMapper::Disconnect()
{
	if (!CarOverlay) return;
	
	CarOverlay->RemoveFromParent();
}

void UCarControlMapper::Move(const FInputActionValue& Value)
{
	TargetCar->Move(Value);
}

void UCarControlMapper::Look(const FInputActionValue& Value)
{
	TargetCar->Look(Value);
}

void UCarControlMapper::ShiftPressed() {}
void UCarControlMapper::ShiftReleased() {}
void UCarControlMapper::SpacePressed() {}
void UCarControlMapper::SpaceReleased() {}

void UCarControlMapper::EPressed()
{
	TargetCar->InteractionAct();
}

void UCarControlMapper::RPressed(){}

void UCarControlMapper::ControlPressed()
{
	TargetCar->ChangeCam();
}

void UCarControlMapper::MouseLeftPressed(){}
void UCarControlMapper::MouseLeftReleased(){}
void UCarControlMapper::MouseRightPressed(){}
void UCarControlMapper::MouseRightReleased() {}

UCameraComponent* UCarControlMapper::GetFollowCamera()
{
	return TargetCar->GetFollowCamera();
}
