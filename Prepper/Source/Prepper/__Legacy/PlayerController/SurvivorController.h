// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasePlayerController.h"
#include "SurvivorController.generated.h"

class UQuickSlotUI;
class UCombatComponent;
enum class EWeaponType : uint8;
class UItemCombinationUI;
class UInventoryUI;
class UStatusWidget;
class UInputAction;
/**
 * 
 */
UCLASS()
class PREPPER_API ASurvivorController : public ABasePlayerController
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = "Player HUD")
	TSubclassOf<UStatusWidget> StatusWidgetClass;
	UPROPERTY()
	TObjectPtr<UStatusWidget> StatusWidget;

	UPROPERTY(EditAnywhere, Category = "Player HUD")
	TSubclassOf<UInventoryUI> InventoryWidgetClass;
	UPROPERTY()
	TObjectPtr<UInventoryUI> InventoryWidget;
	
	UPROPERTY(EditAnywhere, Category = "Player HUD")
	TSubclassOf<UQuickSlotUI> QuickSlotWidgetClass;
	UPROPERTY()
	TObjectPtr<UQuickSlotUI> QuickSlotWidget;
	
	UPROPERTY(EditAnywhere, Category = "Player HUD")
	TSubclassOf<UUserWidget> ClearWidgetClass;
	
	UPROPERTY(EditAnywhere, Category = "Player HUD")
	TSubclassOf<UItemCombinationUI> ItemCombinationClass;


	UFUNCTION(Server, Reliable)
	void ServerAddItem(const FString& ItemCode, int Count);
	UFUNCTION(Server, Reliable)
	void ServerSetAmmo(EWeaponType Type, int Count);
	UFUNCTION(Server, Reliable)
	void ServerEquipEquipment(const FString& EquipmentCode);
	
	
public:
	virtual void BeginPlay() override;
	void LoadGame();
	void SaveGame();
	void Respawn();
	UFUNCTION(Server, Reliable)
	void ServerRespawnRequest(ACharacter* ElimmedCharacter, AController* ElimmedController);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastShowGameEnd();

private:
	void LoadClientData();
	void LoadServerData();
	void SaveClientData();
	void SaveServerData();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = true))
	UInputAction* Button1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = true))
	UInputAction* Button2;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = true))
	UInputAction* Button3;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = true))
	UInputAction* Button4;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = true))
	UInputAction* Button5;
	
	virtual void BeginWidget() override;

	virtual void ServerPossessNewPlayerCharacter() override;
	virtual void LocalPossessNewPlayerCharacter() override;
	virtual void SetInput(UEnhancedInputComponent* Input) override;
	
	void OpenInventoryPressed();

	void QuickSlot1Use();
	void QuickSlot2Use();
	void QuickSlot3Use();
	void QuickSlot4Use();
	void QuickSlot5Use();

	UFUNCTION(Server, Reliable)
	void ServerToggleInventory();
};
