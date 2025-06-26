#include "ItemBackpack.h"
#include "Prepper/Prepper.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/StaticMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Prepper/GamePlay/CharacterController/CharacterController.h"
#include "Prepper/GamePlay/Object/OpenedInventory.h"
#include "Prepper/Unreal/Inventory/UnrealInventoryComponent.h"
#include "Prepper/___Legacy/Character/PlayerCharacter.h"
#include "Sound/SoundCue.h"


AItemBackpack::AItemBackpack()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	BackpackMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BackpackMesh"));
	SetRootComponent(BackpackMesh);

	BackpackMesh->SetCollisionResponseToAllChannels(ECR_Block);
	BackpackMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	BackpackMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	BackpackMesh->SetCollisionResponseToChannel(ECC_Vehicle, ECR_Ignore);
	BackpackMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BackpackMesh->SetRenderCustomDepth(true);
	BackpackMesh->SetCustomDepthStencilValue(CustomDepthColor);

	AreaBox = CreateDefaultSubobject<UBoxComponent>("AreaBox");
	AreaBox->SetupAttachment(RootComponent);
	AreaBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	AreaBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	PickUpWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickUpWidget"));
	PickUpWidget->SetupAttachment(RootComponent);

	Inventory = CreateDefaultSubobject<UUnrealInventoryComponent>(TEXT("Inventory"));
}

void AItemBackpack::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AItemBackpack, BackpackState);
	DOREPLIFETIME(AItemBackpack, PlayerOwnerCharacter);
	DOREPLIFETIME(AItemBackpack, IsOpened);
}

void AItemBackpack::BeginPlay()
{
	Super::BeginPlay();

	CustomDepthColor = CUSTOM_DEPTH_MINT;

	BackpackMesh->SetCustomDepthStencilValue(CustomDepthColor);
	BackpackMesh->MarkRenderStateDirty();
	ToggleOutline(true);
}

void AItemBackpack::Interaction(APlayerCharacter* Target)
{
	if(!Target) return;
	
	PlayerOwnerCharacter = Target;
	SetOwner(Target);
	
	PlayerOwnerCharacter->EquipBackpack(this);
	Inventory->SetOwner(PlayerOwnerCharacter);
	
	if(IsOpened)
		HideInventory();
	
	if (!EquipSound) return;
	
	UGameplayStatics::PlaySoundAtLocation(
		this,
		EquipSound,
		GetActorLocation()
	);
}

void AItemBackpack::Interaction(ICharacterController* Target)
{
	if(!Target) return;
	Target->EquipBackpack(this);
}

void AItemBackpack::Equip()
{
	BackpackPhysicsActive(false);
	
	TObjectPtr<ABaseCharacter> OwnerCharacter = Cast<ABaseCharacter>(GetOwner());
	if (!OwnerCharacter) return;
	OwnerCharacter->AttachActorAtSocket(FName("BackpackSocket"), this);
}

void AItemBackpack::Dropped()
{
	// TODO 
	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
	BackpackMesh->DetachFromComponent(DetachRules);
	BackpackPhysicsActive(true);
	SetOwner(nullptr);
}


void AItemBackpack::OnRep_BackpackState()
{
	OnBackPackState();
}

void AItemBackpack::OnBackPackState()
{
	switch (BackpackState)
	{
	case EBackpackState::EBS_Equipped:
		UE_LOG(LogTemp,Warning,TEXT("Backpack equipped"));
		Equip();
		break;
	case EBackpackState::EBS_Dropped:
		UE_LOG(LogTemp,Warning,TEXT("Backpack dropped"));
		Dropped();
		break;
	case EBackpackState::EBS_OpenInventory:
		UE_LOG(LogTemp,Warning,TEXT("Backpack dropped"));
		BackpackPhysicsActive(false);
		SetActorEnableCollision(true);
		ToggleOutline(true);
		SetOwner(nullptr);
		break;

	default:
		break;
	}
}

void AItemBackpack::SetBackpackState(EBackpackState NewBackpackState)
{
	BackpackState = NewBackpackState;
	OnBackPackState();
}

void AItemBackpack::BackpackPhysicsActive(bool active)
{
	SetActorEnableCollision(active);
	BackpackMesh->SetSimulatePhysics(active);
	BackpackMesh->SetEnableGravity(active);

	ToggleOutline(active);
	ToggleTrigger(active);
	
	if (active)
	{
		BackpackMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		BackpackMesh->SetCollisionResponseToChannels(ColliderChannel);
		return;
	}
	
	BackpackMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AItemBackpack::OpenInventory()
{
	ShowInventory();
	IsOpened = true;
}

void AItemBackpack::ToggleInventory()
{

	UE_LOG(LogTemp,Warning,TEXT("Backpack : Toggle"));
	if(IsOpened)
	{
		HideInventory();
	}
	else
	{
		ShowInventory();
	}
	IsOpened = !IsOpened;
}

void AItemBackpack::ShowInventory()
{
	UE_LOG(LogTemp,Warning,TEXT("Show Inventory"));
	UWorld* World = GetWorld();
	if(!World) return;
	
	if(!PlayerOwnerCharacter) return;
	if(!OpenedInventoryClass) return;

	FRotator SpawnRotation(0.0f, GetActorRotation().Yaw, 0.0f);
	OpenedInventory = World->SpawnActor<AOpenedInventory>(
		OpenedInventoryClass,
		GetActorLocation() + GetActorForwardVector() * 200.0f + GetActorLocation().DownVector * 200.0f,
		SpawnRotation
		);
	OpenedInventory->SetTargetInventory(Inventory);

	const UStaticMeshSocket* TargetSocket = OpenedInventory->GetMesh()->GetSocketByName(TEXT("BackpackSocket"));
	if(TargetSocket)
	{
		TargetSocket->AttachActor(this, OpenedInventory->GetMesh());
	}
	
	SetBackpackState(EBackpackState::EBS_OpenInventory);
}

void AItemBackpack::HideInventory()
{
	UE_LOG(LogTemp,Warning,TEXT("Hide Inventory"));
	if(!OpenedInventory) return;
	OpenedInventory->CloseInventory();
	OpenedInventory->Destroy();
	IsOpened = false;
}
