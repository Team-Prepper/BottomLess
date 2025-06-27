// Fill out your copyright notice in the Description page of Project Settings.


#include "SurvivorGameSaveComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Prepper/GamePlay/Character/BCharacter.h"
#include "Prepper/GamePlay/Character/Component/AmmoBoxComponent.h"
#include "Prepper/GamePlay/Item/Inventory/InventoryComponent.h"
#include "Prepper/Unreal/Controller/PlayerController/UnrealPlayerController.h"
#include "Prepper/_SurvivorMode/SurvivorGameMode.h"
#include "Prepper/_SurvivorMode/GameSave/SurvivorSaveGame.h"
#include "Prepper/_SurvivorMode/GameSave/SurvivorServerSaveGame.h"


// Sets default values for this component's properties
USurvivorGameSaveComponent::USurvivorGameSaveComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicated(true);
	
}

void USurvivorGameSaveComponent::GameSave()
{
	if (GetOwner()->HasAuthority())
	{
		SaveServerData();
	}
	SaveClientData();

	UE_LOG(LogTemp, Warning, TEXT("Data Saved"));
}

void USurvivorGameSaveComponent::SaveClientData()
{
	const TObjectPtr<USurvivorSaveGame> SaveGameInstance =
		Cast<USurvivorSaveGame>(UGameplayStatics::CreateSaveGameObject(USurvivorSaveGame::StaticClass()));

	const TObjectPtr<AUnrealPlayerController> TargetCC = GetOwner<AUnrealPlayerController>();
	
	if (SaveGameInstance)
	{
		SaveGameInstance->Equipments = TargetCC->GetTargetCharacter()->GetEquipmentCodes();
		
		SaveGameInstance->CarriedAmmoMap = TargetCC->GetAmmoBox()->GetAmmoMap();

		TArray<FItemConvertData> ItemData = TargetCC->GetTargetCharacter()->GetInventory()->GetIter();

		for (int i = 0; i < ItemData.Num(); i++)
		{
			SaveGameInstance->InventoryItemCode.Add(ItemData[i].ItemCode);
			SaveGameInstance->InventoryItemCount.Add(ItemData[i].Count);
		}
		
		TArray<FItemConvertData> QuickSlotData = TargetCC->GetTargetCharacter()->GetInventory()->GetQuickSlotIter();
		for (int i = 0; i < QuickSlotData.Num(); i++)
		{
			SaveGameInstance->QuickSlotItemCode.Add(QuickSlotData[i].ItemCode);
			SaveGameInstance->QuickSlotItemCount.Add(QuickSlotData[i].Count);
		}
		
	}
	
	UGameplayStatics::SaveGameToSlot(SaveGameInstance, "Test", 0);
	
}

void USurvivorGameSaveComponent::SaveServerData()
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

