// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GUI.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UGUI : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class IGUI
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void SetOn() PURE_VIRTUAL();
	virtual void SetOff() PURE_VIRTUAL();

	virtual void Open() PURE_VIRTUAL();
	virtual void Close() PURE_VIRTUAL();
};
