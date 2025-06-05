#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "UObject/Interface.h"
#include "ControlMapper.h"
#include "CarControlMapper.generated.h"

class ACarPawn;
class UUserWidget;

UCLASS(hidecategories = Object, Blueprintable, BlueprintType)
class PREPPER_API UCarControlMapper: public UObject, public IControlMapper
{
	GENERATED_BODY()
private:
	UPROPERTY(EditAnywhere, Category="Widget")
	TSubclassOf<UUserWidget> CarOverlayClass;
	UPROPERTY()
	UUserWidget* CarOverlay;
	
	void ToggleControlWidget(bool Toggle, APlayerController* TargetController);
	
public:
	UCarControlMapper():
		CarOverlay(nullptr), TargetCar(nullptr) { } ;
	
	UPROPERTY()
	ACarPawn* TargetCar;

	virtual void Connect(APlayerController* TargetController) override;
	virtual void Disconnect() override;
	
	virtual void Move(const FInputActionValue& Value) override;
	virtual void Look(const FInputActionValue& Value) override;

	virtual void ShiftPressed() override;
	virtual void ShiftReleased() override;

	virtual void SpacePressed() override;
	virtual void SpaceReleased() override;
	virtual void EPressed() override;
	virtual void RPressed() override;

	virtual void ControlPressed() override;

	virtual void MouseLeftPressed() override;
	virtual void MouseLeftReleased() override;
	virtual void MouseRightPressed() override;
	virtual void MouseRightReleased() override;

	virtual UCameraComponent* GetFollowCamera() override;
};