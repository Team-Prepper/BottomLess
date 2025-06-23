// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LegacyCarPawn.h"
#include "PrepperSportsCar.generated.h"

/**
 *  Sports car wheeled vehicle implementation
 */
UCLASS(abstract)
class PREPPER_API APrepperSportsCar : public ALegacyCarPawn
{
	GENERATED_BODY()
	
public:

	APrepperSportsCar();
};
