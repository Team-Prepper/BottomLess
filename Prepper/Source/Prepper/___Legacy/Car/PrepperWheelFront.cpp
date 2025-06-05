// Copyright Epic Games, Inc. All Rights Reserved.

#include "PrepperWheelFront.h"
#include "UObject/ConstructorHelpers.h"

UPrepperWheelFront::UPrepperWheelFront()
{
	AxleType = EAxleType::Front;
	bAffectedBySteering = true;
	MaxSteerAngle = 40.f;
}