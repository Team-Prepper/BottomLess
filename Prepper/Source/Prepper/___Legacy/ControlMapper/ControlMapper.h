#pragma once

struct FInputActionValue;
class UCameraComponent;

class IControlMapper
{
public:
	virtual ~IControlMapper() = default;

	virtual void Connect(APlayerController* TargetController) = 0;
	virtual void Disconnect() = 0;
	
	virtual void Move(const FInputActionValue& Value) = 0;
	virtual void Look(const FInputActionValue& Value) = 0;

	virtual void ShiftPressed() = 0;
	virtual void ShiftReleased() = 0;

	virtual void SpacePressed() = 0;
	virtual void SpaceReleased() = 0;
	virtual void EPressed() = 0;
	virtual void RPressed() = 0;

	virtual void ControlPressed() = 0;

	virtual void MouseLeftPressed() = 0;
	virtual void MouseLeftReleased() = 0;
	virtual void MouseRightPressed() = 0;
	virtual void MouseRightReleased() = 0;

	virtual UCameraComponent* GetFollowCamera()  = 0;;
};
