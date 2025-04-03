#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "Compass.generated.h"

UCLASS()
class PREPPER_API UCompass : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
	UPROPERTY(meta=(BindWidget))
	class UImage* CompassPoint;

	UPROPERTY()
	float CompassOffset;

	void SetDirection();
	float GetImageWidth() const;

	void SetTargetCamera(UCameraComponent* Target);

	UPROPERTY(EditAnywhere)
	UCameraComponent* PlayerCam;

	FTimerHandle SetDirectionTimerHandle;
	FTimerHandle PlayerPossessTimerHandle;
};
