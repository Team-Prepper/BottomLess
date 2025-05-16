// Fill out your copyright notice in the Description page of Project Settings.


#include "UnrealSurvivorInitialSettingComponent.h"

#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Prepper/GamePlay/Character/BCharacter.h"
#include "Prepper/GamePlay/Character/Component/AmmoBoxComponent.h"
#include "Prepper/GamePlay/Character/Component/BCombatComponent.h"
#include "Prepper/GamePlay/CharacterController/StatusComponent.h"
#include "Prepper/GamePlay/GameMode/SurvivorGameMode.h"
#include "Prepper/GamePlay/GameSave/SurvivorSaveGame.h"
#include "Prepper/GamePlay/GameSave/SurvivorServerSaveGame.h"
#include "Prepper/GamePlay/Item/Inventory/InventoryComponent.h"
#include "Prepper/Unreal/Controller/PlayerController/UnrealPlayerController.h"
#include "Prepper/__Legacy/Equipment/EquipmentManager.h"
#include "Prepper/__Legacy/HUD/UI/CharacterOverlay/StatusWidget.h"
#include "Prepper/__Legacy/HUD/UI/Survivor/QuickSlotWidget.h"

void UUnrealSurvivorInitialSettingComponent::Attach()
{
	Super::Attach();
	
	if (StatusWidget != nullptr)
	{
		//TargetCC->GetTargetCharacter()->GetStatus()->Attach(StatusWidget);
	}

	if (QuickSlotWidget != nullptr)
	{
		TargetCC->GetTargetCharacter()->GetInventory()->Attach(QuickSlotWidget);
	}

	TargetCC->GetTargetCharacter()->GetStatus()
		->StatusTimerStart(TargetCC->GetTargetCharacter()->GetMove());
}

void UUnrealSurvivorInitialSettingComponent::Detach()
{
	Super::Detach();
	
	if (StatusWidget != nullptr)
	{
		//TargetCC->GetTargetCharacter()->GetStatus()->Detach(StatusWidget);
	}

	if (QuickSlotWidget != nullptr)
	{
		TargetCC->GetTargetCharacter()->GetInventory()->Detach(QuickSlotWidget);
	}
}

void UUnrealSurvivorInitialSettingComponent::WidgetSetting(TObjectPtr<AUnrealPlayerController> Target)
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

// Sets default values for this component's properties
UUnrealSurvivorInitialSettingComponent::UUnrealSurvivorInitialSettingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UUnrealSurvivorInitialSettingComponent::Initial(TObjectPtr<AUnrealPlayerController> Target)
{
	Super::Initial(Target);
	LoadClientData();
}

void UUnrealSurvivorInitialSettingComponent::ServerAddItem_Implementation(
	UInventoryComponent* TargetInventory, const FString& ItemCode, const int ItemCount)
{
	TargetInventory->TryAddItem(ItemCode, ItemCount);
}

void UUnrealSurvivorInitialSettingComponent::ServerSetAmmo_Implementation(
	UAmmoBoxComponent* AmmoBox, const TArray<FWeaponConvertData>& AmmoMap)
{
	AmmoBox->SetAmmoMap(AmmoMap);
}

void UUnrealSurvivorInitialSettingComponent::ServerEquipEquipment_Implementation(ABCharacter* Target,
	const FString& EquipmentCode)
{
	return;
	AEquipment* SpawnEquipment =
		EquipmentManager::GetInstance()->SpawnEquipment<AEquipment>(GetWorld(), EquipmentCode);

	if (SpawnEquipment == nullptr) return;
	
	SpawnEquipment->Interaction(Target);
}

void UUnrealSurvivorInitialSettingComponent::LoadClientData()
{
	USurvivorSaveGame* LoadGameInstance =
		Cast<USurvivorSaveGame>(UGameplayStatics::LoadGameFromSlot("Test", 0));

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
		
		ServerSetAmmo(TargetCC->GetAmmoBox(), AmmoArray);
		
		for (auto Code : LoadGameInstance->Equipments)
		{
			ServerEquipEquipment(TargetCC->GetTargetCharacter(), Code);
		}
		
		int QuickSlotIdx = 0;
		
		for (auto Item : LoadGameInstance->QuickSlotItemCode)
		{
			if (LoadGameInstance->QuickSlotItemCount[QuickSlotIdx] < 1) continue;
			ServerAddItem(TargetCC->GetTargetCharacter()->GetInventory(),
				Item, LoadGameInstance->QuickSlotItemCount[QuickSlotIdx]);
			TargetCC->GetTargetCharacter()->GetInventory()->QuickSlotAdd(Item, QuickSlotIdx++);
		}
		
		int ItemIdx = 0;
		for (auto Item : LoadGameInstance->InventoryItemCode)
		{
			ServerAddItem(TargetCC->GetTargetCharacter()->GetInventory(),
				Item, LoadGameInstance->InventoryItemCount[ItemIdx++]);
		}
		
	}
}

void UUnrealSurvivorInitialSettingComponent::LoadServerData()
{
	USurvivorServerSaveGame* LoadGameInstance =
		Cast<USurvivorServerSaveGame>(UGameplayStatics::LoadGameFromSlot(FString::Printf(TEXT("%s-%s"), *GetWorld()->GetMapName(), *FString("Server")), 0));

	if (LoadGameInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("Pos: %f, %f, %f"), LoadGameInstance->LastPosition.X, LoadGameInstance->LastPosition.Y, LoadGameInstance->LastPosition.Z );
		GetOwner<AUnrealPlayerController>()->GetTargetCharacter()->SetActorLocation(LoadGameInstance->LastPosition);
	}
}

void UUnrealSurvivorInitialSettingComponent::SaveClientData()
{
	const TObjectPtr<USurvivorSaveGame> SaveGameInstance =
		Cast<USurvivorSaveGame>(UGameplayStatics::CreateSaveGameObject(USurvivorSaveGame::StaticClass()));

	if (SaveGameInstance)
	{
		//SaveGameInstance->Equipments = GetOwner<AUnrealPlayerController>()->GetTargetCharacter()->GetEquipmentCodes();
		
		SaveGameInstance->CarriedAmmoMap = TargetCC->GetAmmoBox()->GetAmmoMap();

		TArray<FItemConvertData> ItemData = GetOwner<AUnrealPlayerController>()->GetTargetCharacter()->GetInventory()->GetIter();

		for (int i = 0; i < ItemData.Num(); i++)
		{
			SaveGameInstance->InventoryItemCode.Add(ItemData[i].ItemCode);
			SaveGameInstance->InventoryItemCount.Add(ItemData[i].Count);
		}
		
		TArray<FItemConvertData> QuickSlotData = GetOwner<AUnrealPlayerController>()->GetTargetCharacter()->GetInventory()->GetQuickSlotIter();
		for (int i = 0; i < QuickSlotData.Num(); i++)
		{
			SaveGameInstance->QuickSlotItemCode.Add(QuickSlotData[i].ItemCode);
			SaveGameInstance->QuickSlotItemCount.Add(QuickSlotData[i].Count);
		}
		
	}
	
	UGameplayStatics::SaveGameToSlot(SaveGameInstance, "Test", 0);
	
}

void UUnrealSurvivorInitialSettingComponent::SaveServerData()
{
	ASurvivorGameMode* GM = GetWorld()->GetAuthGameMode<ASurvivorGameMode>();
	
	if (GM == nullptr) return;
	
	const TObjectPtr<USurvivorServerSaveGame> SaveGameInstance =
		Cast<USurvivorServerSaveGame>(UGameplayStatics::CreateSaveGameObject(USurvivorServerSaveGame::StaticClass()));

	if (SaveGameInstance)
	{
		SaveGameInstance->PlayTime = GM->GetPlayTime();
		SaveGameInstance->LastPosition = GetOwner<AUnrealPlayerController>()->GetTargetCharacter()->GetActorLocation();
		SaveGameInstance->Achievement = GM->GetAchievement();
	}
	
	UGameplayStatics::SaveGameToSlot(SaveGameInstance, "Server", 0);
}

