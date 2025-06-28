// Fill out your copyright notice in the Description page of Project Settings.


#include "SurvivorInitialSettingComponent.h"

#include "Blueprint/UserWidget.h"
#include "EntitySystem/MovieSceneEntitySystemRunner.h"
#include "Kismet/GameplayStatics.h"
#include "Prepper/GamePlay/Character/BCharacter.h"
#include "Prepper/GamePlay/Character/Component/AmmoBoxComponent.h"
#include "Prepper/GamePlay/Character/Component/Status/StatusComponent.h"
#include "Prepper/GamePlay/Equipment/EquipmentManager.h"
#include "Prepper/GamePlay/Item/Inventory/InventoryComponent.h"
#include "Prepper/Unreal/Controller/PlayerController/UnrealPlayerController.h"
#include "Prepper/_SurvivorMode/GameSave/SurvivorSaveGame.h"
#include "Prepper/_SurvivorMode/GameSave/SurvivorServerSaveGame.h"
#include "Prepper/_SurvivorMode/UI/StatusWidget.h"
#include "Prepper/_SurvivorMode/UI/QuickSlotWidget.h"

// Sets default values for this component's properties
USurvivorInitialSettingComponent::USurvivorInitialSettingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicated(true);
	// ...
}

void USurvivorInitialSettingComponent::Attach()
{
	Super::Attach();
	
	if (StatusWidget != nullptr)
	{
		TargetCC->GetTargetCharacter()->GetStatus()->Attach(StatusWidget);
	}

	if (QuickSlotWidget != nullptr)
	{
		TargetCC->GetTargetCharacter()->GetInventory()->Attach(QuickSlotWidget);
	}

	TargetCC->GetTargetCharacter()->GetStatus()
		->StatusTimerStart(TargetCC->GetTargetCharacter()->GetMove());
}

void USurvivorInitialSettingComponent::Detach()
{
	Super::Detach();
	
	if (StatusWidget != nullptr)
	{
		TargetCC->GetTargetCharacter()->GetStatus()->Detach(StatusWidget);
	}

	if (QuickSlotWidget != nullptr)
	{
		TargetCC->GetTargetCharacter()->GetInventory()->Detach(QuickSlotWidget);
	}
}

void USurvivorInitialSettingComponent::WidgetSetting(TObjectPtr<AUnrealPlayerController> Target)
{
	
	if (StatusWidgetClass && StatusWidget == nullptr)
	{
		StatusWidget = CreateWidget<UStatusWidget>(Target, StatusWidgetClass);
		StatusWidget->AddToViewport();
	}
	
	if (QuickSlotWidgetClass && QuickSlotWidget == nullptr)
	{
		QuickSlotWidget = CreateWidget<UQuickSlotUI>(Target, QuickSlotWidgetClass);
		QuickSlotWidget->AddToViewport();
	}
	
	Super::WidgetSetting(Target);
}

void USurvivorInitialSettingComponent::Initial(TObjectPtr<AUnrealPlayerController> Target)
{
	Super::Initial(Target);

	if (Target->HasAuthority())
	{
		LoadServerData(Target);
	}
	if (Target->IsLocalController())
	{
		LoadClientData(Target);
	}
	
}

void USurvivorInitialSettingComponent::ServerAddItem_Implementation(
	UInventoryComponent* TargetInventory, const FString& ItemCode, const int ItemCount)
{
	TargetInventory->TryAddItem(ItemCode, ItemCount);
}

void USurvivorInitialSettingComponent::ServerSetAmmo_Implementation(
	UAmmoBoxComponent* AmmoBox, const TArray<FWeaponConvertData>& AmmoMap)
{
	AmmoBox->SetAmmoMap(AmmoMap);
}

void USurvivorInitialSettingComponent::ServerEquipEquipment_Implementation(ABCharacter* Target,
	const FString& EquipmentCode)
{
	const TObjectPtr<AEquipment> SpawnEquipment =
		EquipmentManager::GetInstance()->SpawnEquipment<AEquipment>(GetWorld(), EquipmentCode);

	UE_LOG(LogTemp, Warning, TEXT("Equip Try"));
	if (SpawnEquipment == nullptr) return;

	UE_LOG(LogTemp, Warning, TEXT("Equip %s"), *SpawnEquipment->GetName());
	if (Target == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("SERVER: TargetNULL"));
	}
	SpawnEquipment->Interaction(Target);
}

void USurvivorInitialSettingComponent::LoadClientData(TObjectPtr<AUnrealPlayerController> Target)
{
	USurvivorSaveGame* LoadGameInstance =
		Cast<USurvivorSaveGame>(UGameplayStatics::LoadGameFromSlot("Test", 0));

	if (Target == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("CLIENT: TargetNULL"));
	}
	if (LoadGameInstance)
	{
		TArray<FWeaponConvertData> AmmoArray;
		
		for (const auto& Elem : LoadGameInstance->CarriedAmmoMap)
		{
			FWeaponConvertData ItemData;
			ItemData.WeaponType = Elem.Key;
			ItemData.Count = Elem.Value;
			AmmoArray.Add(ItemData);
		}
		
		ServerSetAmmo(Target->GetAmmoBox(), AmmoArray);
		
		for (auto Code : LoadGameInstance->Equipments)
		{
			ServerEquipEquipment(Target->GetTargetCharacter(), Code);
		}
		
		int QuickSlotIdx = 0;
		
		for (auto Item : LoadGameInstance->QuickSlotItemCode)
		{
			if (LoadGameInstance->QuickSlotItemCount[QuickSlotIdx] < 1) continue;
			ServerAddItem(Target->GetTargetCharacter()->GetInventory(),
				Item, LoadGameInstance->QuickSlotItemCount[QuickSlotIdx]);
			Target->GetTargetCharacter()->GetInventory()->QuickSlotAdd(Item, QuickSlotIdx++);
		}
		
		int ItemIdx = 0;
		for (auto Item : LoadGameInstance->InventoryItemCode)
		{
			ServerAddItem(Target->GetTargetCharacter()->GetInventory(),
				Item, LoadGameInstance->InventoryItemCount[ItemIdx++]);
		}
		
	}
}

void USurvivorInitialSettingComponent::LoadServerData(TObjectPtr<AUnrealPlayerController> Target)
{
	USurvivorServerSaveGame* LoadGameInstance =
		Cast<USurvivorServerSaveGame>(UGameplayStatics::LoadGameFromSlot(FString::Printf(TEXT("%s-%s"), *GetWorld()->GetMapName(), *FString("Server")), 0));

	if (LoadGameInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("Pos: %f, %f, %f"), LoadGameInstance->LastPosition.X, LoadGameInstance->LastPosition.Y, LoadGameInstance->LastPosition.Z );
		Target->GetTargetCharacter()->SetActorLocation(LoadGameInstance->LastPosition);
	}
}
