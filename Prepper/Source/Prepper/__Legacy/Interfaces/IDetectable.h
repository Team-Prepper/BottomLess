#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IDetectable.generated.h"

// 감지 가능한 타입을 정의
UENUM(BlueprintType)
enum class EDetectableType : uint8
{
	Item,
	Weapon,
	Enemy,
	Location,
	None
};


UINTERFACE(MinimalAPI)
class UDetectable : public UInterface
{
	GENERATED_BODY()
};

class PREPPER_API IDetectable
{
	GENERATED_BODY()

public:
	// 감지 가능한 객체가 제공해야 할 정보
	virtual FString GetDetectableDescription() const = 0;
	virtual EDetectableType GetDetectableType() const = 0;
};
