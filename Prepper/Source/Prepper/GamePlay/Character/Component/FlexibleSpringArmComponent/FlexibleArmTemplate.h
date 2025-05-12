#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "FlexibleArmTemplate.generated.h"

/**
 * 
 */
USTRUCT()
struct PREPPER_API FFlexibleArmTemplate
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	float Length;

	UPROPERTY(EditAnywhere)
	FVector Location;

	// 생성자 선언
	FFlexibleArmTemplate()
		: Length(100.0f), Location(FVector::ZeroVector)
	{
	}

	FFlexibleArmTemplate(float InLength, FVector InLocation)
		: Length(InLength), Location(InLocation)
	{
	}
};