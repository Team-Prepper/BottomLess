#include "WeaponBoxSpawner.h"
#include "Net/UnrealNetwork.h"
#include "Prepper/GamePlay/Item/Object/InteractableBox.h"

// Sets default values
AWeaponBoxSpawner::AWeaponBoxSpawner()
{
	PrimaryActorTick.bCanEverTick = true;
	FallingSpeed = 100.0f;
	bReplicates = true;
}

void AWeaponBoxSpawner::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AWeaponBoxSpawner, ControlledFallingActor);
}

void AWeaponBoxSpawner::BeginPlay()
{
	Super::BeginPlay();
	GetWorld()->GetTimerManager().SetTimer(
		InitialDelayTimerHandle,
		this,
		&AWeaponBoxSpawner::ExecutePeriodicFunction,
		FirstTimeToSpawn,  // 초기 지연 시간 (120초)
		false    // 한 번만 실행
	);
	
}

void AWeaponBoxSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWeaponBoxSpawner::ExecutePeriodicFunction()
{
	if (HasAuthority())
	{
		FVector RandomLocation = FVector(
			FMath::RandRange(MinSpawnRange.X, MaxSpawnRange.X),
			FMath::RandRange(MinSpawnRange.Y, MaxSpawnRange.Y),
			FMath::RandRange(MinSpawnRange.Z, MaxSpawnRange.Z)
		);
		
		SpawnFallingActor_Server(RandomLocation);
	}
	
	GetWorld()->GetTimerManager().SetTimer(
		PeriodicTimerHandle,
		this,
		&AWeaponBoxSpawner::ExecutePeriodicFunction,
		SpawnRange,   // 반복 주기 (20초)
		true     // 반복 실행
	);
}

void AWeaponBoxSpawner::SpawnFallingActor_Server_Implementation(FVector SpawnLocation)
{
	if (FallingActorClass)
	{
		FRotator SpawnRotation = FRotator::ZeroRotator;

		ControlledFallingActor = GetWorld()->SpawnActor<AInteractableBox>(FallingActorClass, SpawnLocation, SpawnRotation);
		if (ControlledFallingActor)
		{
			ControlledFallingActor->SetReplicates(true);
			Multicast_DropActor();
		}
	}
}

void AWeaponBoxSpawner::Multicast_DropActor_Implementation()
{
	if (ControlledFallingActor && ControlledFallingActor->BoxMesh)
	{
		ControlledFallingActor->BoxMesh->SetSimulatePhysics(true);
		ControlledFallingActor->BoxMesh->SetEnableGravity(true);

		// 클라이언트에서도 물리 시뮬레이션이 활성화되도록 클라이언트 함수 호출
		ControlledFallingActor->BoxMesh->SetIsReplicated(true);
	}
}
