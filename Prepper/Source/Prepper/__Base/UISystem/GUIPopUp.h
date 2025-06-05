// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GUI.h"
#include "GUIPopUp.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UGUIPopUp : public UInterface
{
	GENERATED_BODY()
};

class PREPPER_API IGUIPopUp : public IGUI
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
};
