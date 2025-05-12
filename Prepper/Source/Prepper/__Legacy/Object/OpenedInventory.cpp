#include "OpenedInventory.h"

#include "Prepper/GamePlay/Item/ItemManager.h"
#include "Prepper/GamePlay/Item/Inventory/InventoryComponent.h"
#include "Prepper/GamePlay/Item/Object/InventoryInteractableItem.h"

AOpenedInventory::AOpenedInventory()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	BaseActor = CreateDefaultSubobject<USceneComponent>(TEXT("BaseActor"));
	SetRootComponent(BaseActor);

	InventoryMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("InventoryMesh"));
	InventoryMesh->SetupAttachment(RootComponent);
}

void AOpenedInventory::SetTargetInventory(UInventoryComponent* Inventory)
{
	TargetInventory = Inventory;
	InitInventory();
}

void AOpenedInventory::InitInventory()
{
	if (!HasAuthority()) return;

	UE_LOG(LogTemp, Warning, TEXT("Opened Inven init"));
	
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("NO WORLD"));
		return;
	}
	
	const FRotator SpawnRotation(0.0f, GetActorRotation().Yaw, 0.0f);

	TArray<FItemConvertData> Items = TargetInventory->GetIter();
	
	//UE_LOG(LogTemp, Warning, TEXT("Item Count : %d"), Items.Num());
	for (int i = 0; i < Items.Num(); i++)
	{

		InventoryInteractableItem = ItemManager::GetInstance()->SpawnItemInteraction(GetWorld(), Items[i].ItemCode);
	
		if (!InventoryInteractableItem)
		{
			UE_LOG(LogTemp, Warning, TEXT("Check Game Instace!"));
			continue;
		}
		
		InventoryInteractableItem->SetActorLocation(GetActorLocation()
			+ GetActorForwardVector() * (RowPivot - 100.0f * (i % RowSize))
			+ GetActorRightVector() * (ColPivot + 100.0f * (i / RowSize))
			+ GetActorLocation().UpVector * HeightPivot);

		InventoryInteractableItem->SetActorRotation(SpawnRotation);
		InventoryInteractableItem->SetTargetInventory(TargetInventory);
		
		SpawnedActors.Add(InventoryInteractableItem);
		
		UE_LOG(LogTemp, Warning, TEXT("Spawn Inven Item"));
	}
}

void AOpenedInventory::CloseInventory()
{
	for (auto Item : SpawnedActors)
	{
		Item->Destroy();
	}
}
