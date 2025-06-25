// Fill out your copyright notice in the Description page of Project Settings.


#include "SurvivorController.h"

#include "EnhancedInputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Prepper/GamePlay/PrepperGameMode.h"
#include "Prepper/GamePlay/Equipment/EquipmentManager.h"
#include "Prepper/GamePlay/Item/Inventory/InventoryComponent.h"
#include "Prepper/GamePlay/Item/Inventory/UI/InventoryUI.h"
#include "Prepper/_SurvivorMode/SurvivorGameMode.h"
#include "Prepper/_SurvivorMode/GameSave/SurvivorSaveGame.h"
#include "Prepper/_SurvivorMode/GameSave/SurvivorServerSaveGame.h"
#include "Prepper/_SurvivorMode/UI/QuickSlotWidget.h"
#include "Prepper/_SurvivorMode/UI/StatusWidget.h"
#include "Prepper/___Legacy/Character/Component/StatusEffectComponent.h"
#include "Prepper/___Legacy/Character/Component/Combat/CombatComponent.h"
#include "Prepper/___Legacy/HUD/PrepperHUD.h"

void ASurvivorController::BeginWidget()
{
	if (StatusWidgetClass && StatusWidget == nullptr)
	{
		StatusWidget = CreateWidget<UStatusWidget>(this, StatusWidgetClass);
		StatusWidget->AddToViewport();
	}
	
	if (QuickSlotWidgetClass && QuickSlotWidget == nullptr)
	{
		QuickSlotWidget = CreateWidget<UQuickSlotUI>(this, QuickSlotWidgetClass);
		QuickSlotWidget->AddToViewport();
	}
	
	if (InventoryWidgetClass && InventoryWidget == nullptr)
	{
		InventoryWidget = CreateWidget<UInventoryUI>(this, InventoryWidgetClass);
		InventoryWidget->SetVisibility(ESlateVisibility::Hidden);
		InventoryWidget->AddToViewport();
	}
	
	Super::BeginWidget();
	
}

void ASurvivorController::ServerPossessNewPlayerCharacter()
{
	Super::ServerPossessNewPlayerCharacter();
	UE_LOG(LogTemp, Warning, TEXT("ServerPossessNewPlayerCharacter"));
	LoadServerData();
}

void ASurvivorController::LocalPossessNewPlayerCharacter()
{
	Super::LocalPossessNewPlayerCharacter();

	if (!PlayerCharacter) return;
	if (!CharacterOverlay) return;
	if (!StatusWidget) return;
	if (!PlayerCharacter->GetStatusEffectComponent()) return;
	
	LoadClientData();

	UE_LOG(LogTemp, Warning, TEXT("Attach UI"));
	
	PlayerCharacter->GetStatusEffectComponent()->Attach(StatusWidget);
	PlayerCharacter->GetInventory()->Attach(QuickSlotWidget);
	PlayerCharacter->GetStatusEffectComponent()->StatusTimerStart();
	InventoryWidget->SetTargetPlayer(PlayerCharacter);
}

void ASurvivorController::SetInput(UEnhancedInputComponent* Input)
{
	Super::SetInput(Input);
	
	Input->BindAction(OpenInventory, ETriggerEvent::Triggered, this, &ASurvivorController::OpenInventoryPressed);
	Input->BindAction(Button1, ETriggerEvent::Started, this, &ASurvivorController::QuickSlot1Use);
	Input->BindAction(Button2, ETriggerEvent::Started, this, &ASurvivorController::QuickSlot2Use);
	Input->BindAction(Button3, ETriggerEvent::Started, this, &ASurvivorController::QuickSlot3Use);
	Input->BindAction(Button4, ETriggerEvent::Started, this, &ASurvivorController::QuickSlot4Use);
	Input->BindAction(Button5, ETriggerEvent::Started, this, &ASurvivorController::QuickSlot5Use);
	
}

void ASurvivorController::OpenInventoryPressed()
{
	if (!InventoryWidget) return;
	InventoryWidget->SetVisibility(ESlateVisibility::Visible);
	//ServerToggleInventory();
}

void ASurvivorController::ServerToggleInventory_Implementation()
{
	/*
	if (!TargetControllerable) return;
	TargetControllerable->ToggleInventory();
	*/
}

void ASurvivorController::QuickSlot1Use()
{
	UE_LOG(LogTemp, Warning, TEXT("Button1Pressed"));
	
	if (!PlayerCharacter) return;
	PlayerCharacter->UseQuickSlotItem(0);
	
	return;
}

void ASurvivorController::QuickSlot2Use()
{
	UE_LOG(LogTemp, Warning, TEXT("Button2Pressed"));
	if (!PlayerCharacter) return;
	PlayerCharacter->UseQuickSlotItem(1);
}

void ASurvivorController::QuickSlot3Use()
{
	UE_LOG(LogTemp, Warning, TEXT("Button3Pressed"));
	if (!PlayerCharacter) return;
	PlayerCharacter->UseQuickSlotItem(2);
}

void ASurvivorController::QuickSlot4Use()
{
	UE_LOG(LogTemp, Warning, TEXT("Button4Pressed"));
	if (!PlayerCharacter) return;
	PlayerCharacter->UseQuickSlotItem(3);
}

void ASurvivorController::QuickSlot5Use()
{
	UE_LOG(LogTemp, Warning, TEXT("Button5Pressed"));
	if (!PlayerCharacter) return;
	PlayerCharacter->UseQuickSlotItem(4);
}

void ASurvivorController::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority()) return;
	if (GetPawn() != nullptr && !GetPawn()->IsLocallyControlled()) return;

	USurvivorServerSaveGame* LoadGameInstance =
		Cast<USurvivorServerSaveGame>(UGameplayStatics::LoadGameFromSlot(FString::Printf(TEXT("%s-%s"), *GetWorld()->GetMapName(), *FString("Server")), 0));

	if (!LoadGameInstance) return;

	if (ASurvivorGameMode* GM = GetWorld()->GetAuthGameMode<ASurvivorGameMode>())
	{
		GM->SetPlayTime(LoadGameInstance->PlayTime);
		for (auto Str : LoadGameInstance->Achievement)
		{
			GM->AddAchievement(Str.Key, Str.Value);
		}
	}
	
}

void ASurvivorController::LoadGame()
{
}

void ASurvivorController::ServerAddItem_Implementation(const FString& ItemCode, int Count)
{
	PlayerCharacter->GetInventory()->TryAddItem(ItemCode, Count);
}

void ASurvivorController::ServerSetAmmo_Implementation(EWeaponType Type, int Count)
{
	UCombatComponent* Target =
			Cast<UCombatComponent>(PlayerCharacter->GetCombatComponent());

	if (Target == nullptr) return;
	
	if (Target->CarriedAmmoMap.Contains(Type))
	{
		Target->CarriedAmmoMap[Type] = Count;
		return;
	}
	
	Target->CarriedAmmoMap.Add(Type, Count);
	
}

void ASurvivorController::ServerEquipEquipment_Implementation(const FString& EquipmentCode)
{
	
	AEquipment* SpawnEquipment =
		EquipmentManager::GetInstance()->SpawnEquipment<AEquipment>(GetWorld(), EquipmentCode);

	if (SpawnEquipment == nullptr) return;
	
	SpawnEquipment->Interaction(PlayerCharacter);
	
}


void ASurvivorController::SaveGame()
{
	SaveClientData();
	SaveServerData();
}

void ASurvivorController::Respawn()
{
}

void ASurvivorController::ServerRespawnRequest_Implementation(ACharacter* ElimmedCharacter,
	AController* ElimmedController)
{
	const TObjectPtr<APrepperGameMode> GM = GetWorld()->GetAuthGameMode<APrepperGameMode>();
	if (GM == nullptr) return;
	GM->RequestRespawn(GetPawn<APlayerCharacter>(), this);
}

void ASurvivorController::MulticastShowGameEnd_Implementation()
{
	if (!IsLocalController()) return;
	if (!ClearWidgetClass) return;
	
	CreateWidget(this, ClearWidgetClass)->AddToViewport();
}

void ASurvivorController::LoadClientData()
{
	USurvivorSaveGame* LoadGameInstance =
		Cast<USurvivorSaveGame>(UGameplayStatics::LoadGameFromSlot("Test", 0));

	if (LoadGameInstance)
	{
		for (auto Value : LoadGameInstance->CarriedAmmoMap)
		{
			ServerSetAmmo(Value.Key, Value.Value);
		}
		for (auto Code : LoadGameInstance->Equipments)
		{
			ServerEquipEquipment(Code);
		}
		
		int QuickSlotIdx = 0;
		for (auto Item : LoadGameInstance->QuickSlotItemCode)
		{
			if (LoadGameInstance->QuickSlotItemCount[QuickSlotIdx] < 1) continue;
			ServerAddItem(Item, LoadGameInstance->QuickSlotItemCount[QuickSlotIdx]);
			PlayerCharacter->GetInventory()->QuickSlotAdd(Item, QuickSlotIdx++);
		}
		int ItemIdx = 0;
		for (auto Item : LoadGameInstance->InventoryItemCode)
		{
			ServerAddItem(Item, LoadGameInstance->InventoryItemCount[ItemIdx++]);
		}
		
	}
}

void ASurvivorController::LoadServerData()
{
	USurvivorServerSaveGame* LoadGameInstance =
		Cast<USurvivorServerSaveGame>(UGameplayStatics::LoadGameFromSlot(FString::Printf(TEXT("%s-%s"), *GetWorld()->GetMapName(), *FString("Server")), 0));

	if (LoadGameInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("Pos: %f, %f, %f"), LoadGameInstance->LastPosition.X, LoadGameInstance->LastPosition.Y, LoadGameInstance->LastPosition.Z );
		PlayerCharacter->SetActorLocation(LoadGameInstance->LastPosition);
	}
}

void ASurvivorController::SaveClientData()
{
	const TObjectPtr<USurvivorSaveGame> SaveGameInstance =
		Cast<USurvivorSaveGame>(UGameplayStatics::CreateSaveGameObject(USurvivorSaveGame::StaticClass()));

	if (SaveGameInstance)
	{
		SaveGameInstance->Equipments = PlayerCharacter->GetEquipmentCodes();
		
		if (UCombatComponent* CombatComp =
			Cast<UCombatComponent>(PlayerCharacter->GetCombatComponent()))
		{
			SaveGameInstance->CarriedAmmoMap = CombatComp->CarriedAmmoMap;
		}

		TArray<FItemConvertData> ItemData = PlayerCharacter->GetInventory()->GetIter();
		for (int i = 0; i < ItemData.Num(); i++)
		{
			SaveGameInstance->InventoryItemCode.Add(ItemData[i].ItemCode);
			SaveGameInstance->InventoryItemCount.Add(ItemData[i].Count);
		}
		
		TArray<FItemConvertData> QuickSlotData = PlayerCharacter->GetInventory()->GetQuickSlotIter();
		for (int i = 0; i < QuickSlotData.Num(); i++)
		{
			SaveGameInstance->QuickSlotItemCode.Add(QuickSlotData[i].ItemCode);
			SaveGameInstance->QuickSlotItemCount.Add(QuickSlotData[i].Count);
		}
		
	}
	
	UGameplayStatics::SaveGameToSlot(SaveGameInstance, "Test", 0);
}

void ASurvivorController::SaveServerData()
{
	ASurvivorGameMode* GM = GetWorld()->GetAuthGameMode<ASurvivorGameMode>();
	if (GM == nullptr) return;
	
	const TObjectPtr<USurvivorServerSaveGame> SaveGameInstance =
		Cast<USurvivorServerSaveGame>(UGameplayStatics::CreateSaveGameObject(USurvivorServerSaveGame::StaticClass()));

	if (SaveGameInstance)
	{
		SaveGameInstance->PlayTime = GM->GetPlayTime();
		SaveGameInstance->LastPosition = PlayerCharacter->GetActorLocation();
		SaveGameInstance->Achievement = GM->GetAchievement();
		
	}
	
	UGameplayStatics::SaveGameToSlot(SaveGameInstance, "Server", 0);
}
