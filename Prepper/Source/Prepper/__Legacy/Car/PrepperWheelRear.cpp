// Copyright Epic Games, Inc. All Rights Reserved.

#include "PrepperWheelRear.h"
#include "UObject/ConstructorHelpers.h"

UPrepperWheelRear::UPrepperWheelRear()
{
	AxleType = EAxleType::Rear;
	bAffectedByHandbrake = true;
	bAffectedByEngine = true;
}