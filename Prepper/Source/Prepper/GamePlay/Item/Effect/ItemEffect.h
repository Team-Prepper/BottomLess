// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class IPlayerAbility;

class PREPPER_API IItemEffect
{
	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual ~IItemEffect() = default;
	virtual void Action(IPlayerAbility* Target) = 0;
};
