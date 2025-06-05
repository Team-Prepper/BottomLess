#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "UObject/Interface.h"
#include "Controllable.generated.h"

class UCameraComponent;
class IControlMapper;

UINTERFACE(MinimalAPI)
class UControllable : public UInterface
{
	GENERATED_BODY()
};

class PREPPER_API IControllable
{
	GENERATED_BODY()
public:
	virtual IControlMapper* GetControlMapper() PURE_VIRTUAL(IControllable::GetControlMapper, return nullptr;  );
	
	virtual void Move(const FInputActionValue& Value) PURE_VIRTUAL(IControllable::Move, );
	virtual void Look(const FInputActionValue& Value) PURE_VIRTUAL(IControllable::Look, );
	
	virtual UCameraComponent* GetFollowCamera() PURE_VIRTUAL(IControllable::GetFollowCamera, return nullptr;);
};