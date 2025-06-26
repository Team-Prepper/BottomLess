// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemPickupAmmo.h"

#include "Prepper/GamePlay/InteractionEventComponent/InteractionAddAmmoEventComponent.h"

AItemPickupAmmo::AItemPickupAmmo()
{
	InteractionEvent = CreateDefaultSubobject<UInteractionAddAmmoEventComponent>(TEXT("AddAmmo"));
}