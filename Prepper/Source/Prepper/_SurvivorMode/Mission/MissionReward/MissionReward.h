// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

class PREPPER_API IMissionReward
{
	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual ~IMissionReward() = default;
	virtual void GetReward() PURE_VIRTUAL();
};
