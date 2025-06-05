#include "EnemyBaseCharacter.h"
#include "AIController.h"
#include "Component/Combat/BaseCombatComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "Prepper/Prepper.h"
#include "Prepper/___Legacy/Weapon/RangeWeapon/RangeWeapon.h"

AEnemyBaseCharacter::AEnemyBaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);
	
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
	PawnSensing->SightRadius = 4000.f;
	PawnSensing->SetPeripheralVisionAngle(100.f);

	CombatComp = CreateDefaultSubobject<UBaseCombatComponent>(TEXT("CombatComponent"));
	CombatComp->SetIsReplicated(true);
	
}

void AEnemyBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	EnemyController = Cast<AAIController>(GetController());
	
	SpawnWeaponActor();
	
	if (PawnSensing)
	{
		PawnSensing->OnSeePawn.AddDynamic(this, &AEnemyBaseCharacter::PawnSeen);
		PawnSensing->OnHearNoise.AddDynamic(this, &AEnemyBaseCharacter::PawnHearn);
	}
}

void AEnemyBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(IsElimed()) return;
	if (CombatComp->GetCombatState() == ECombatState::ECS_Fire) return;
	
	if (!TargetAlive(PatrolTarget))
	{
		PatrolTarget = nullptr;
		EnemyState = EEnemyState::EES_Patrolling;
	}
	
	// 공격사거리 안에서 공격이 아닐떄 -> 공격!
	if (InTargetRange(PatrolTarget, AttackRadius))
	{
		GetCharacterMovement()->StopMovementImmediately();
		//UE_LOG(LogTemp, Warning, TEXT("CODE : zombie Attack"));
		AttackTrigger(true);
		AttackTrigger(false);

		//UE_LOG(LogTemp, Warning, TEXT("%hs"), CombatComp == nullptr ? "True":"False");
		return;
	}

	if (InTargetRange(PatrolTarget, CombatRadius))
	{
		//UE_LOG(LogTemp, Warning, TEXT("Enemy Found Target -> chasing"));
		EnemyState = EEnemyState::EES_Chasing;
		GetCharacterMovement()->MaxWalkSpeed = 600.f;
		MoveToTarget(PatrolTarget);
		return;
	}

	if (PatrolTarget != nullptr)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Lost Target"));
		EnemyState = EEnemyState::EES_Patrolling;
		GetCharacterMovement()->MaxWalkSpeed = 300.f;
		MoveToTarget(PatrolTarget);
	}

	CheckPatrolTarget();
}

void AEnemyBaseCharacter::CheckPatrolTarget()
{
	if(IsElimed()) return;
	if (InTargetRange(PatrolTarget, PatrolRadius))
	{
		PatrolTarget = ChoosePatrolTarget();
		const float WaitTime = FMath::RandRange(WaitMin, WaitMax);
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemyBaseCharacter::PatrolTimerFinished, WaitTime);
	}
}

void AEnemyBaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AEnemyBaseCharacter::ReceiveDamage(float Damage, AController* InstigatorController, AActor* DamageCauser)
{
	Super::ReceiveDamage(Damage, InstigatorController, DamageCauser);
}


void AEnemyBaseCharacter::PatrolTimerFinished()
{
	MoveToTarget(PatrolTarget);
}

bool AEnemyBaseCharacter::InTargetRange(const TObjectPtr<AActor> Target, const float Radius) const
{
	if (Target == nullptr) return false;
	
	const float DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).SizeSquared();
	return DistanceToTarget <= Radius * Radius;
}

bool AEnemyBaseCharacter::TargetAlive(const TObjectPtr<AActor> Target) const
{
	if (!Target) return false;
	if (Target->ActorHasTag("Death")) return false;
	return true;
	
}

void AEnemyBaseCharacter::MoveToTarget(AActor* Target)
{
	if(IsElimed()) return;
	if (EnemyController == nullptr || Target == nullptr) return;
	EnemyController->MoveToActor(Target, 15.f);
}

void AEnemyBaseCharacter::MoveToLocation(FVector& Location)
{
	if(IsElimed()) return;
	if (EnemyController == nullptr || Location == FVector::ZeroVector) return;
	EnemyController->MoveToLocation(Location, 1.5f);
}


AActor* AEnemyBaseCharacter::ChoosePatrolTarget()
{
	TArray<AActor*> ValidTargets;
	for (AActor* Target : PatrolTargets)
	{
		if (Target != PatrolTarget)
		{
			ValidTargets.AddUnique(Target);
		}
	}

	const int32 NumPatrolTargets = ValidTargets.Num();
	if (NumPatrolTargets > 0)
	{
		const int32 TargetSelection = FMath::RandRange(0, NumPatrolTargets - 1);
		return ValidTargets[TargetSelection];
	}
	return nullptr;
}

void AEnemyBaseCharacter::PawnSeen(APawn* SeenPawn)
{
	if(IsElimed()) return;
	if (!SeenPawn->ActorHasTag(FName("PlayerCharacter"))) return;
	if (SeenPawn->ActorHasTag(FName("Death"))) return;
	
	// 엑터의 태그가 플레이어일때만
	//UE_LOG(LogTemp, Warning, TEXT("zombie SEE -> chasing"));
	
	GetWorldTimerManager().ClearTimer(PatrolTimer);
	PatrolTarget = SeenPawn;
	
	if (EnemyState != EEnemyState::EES_Attacking)
	{
		EnemyState = EEnemyState::EES_Chasing;
		FVector TargetLocation = PatrolTarget->GetActorLocation(); // 플레이어의 위치를 복사하여 전달
		MoveToLocation(TargetLocation); // 플레이어 위치로 이동
	}
	
}

void AEnemyBaseCharacter::PawnHearn(APawn *HearnPawn, const FVector &Location, float Volume)
{
	if(IsElimed()) return;
	
	if (EnemyState == EEnemyState::EES_Chasing) return;
	
	if (!HearnPawn->ActorHasTag(FName("PlayerCharacter"))) return;
	if (HearnPawn->ActorHasTag(FName("Death"))) return;
	
	UE_LOG(LogTemp, Display, TEXT("CODE : zombie HEAR"));
	
	GetWorldTimerManager().ClearTimer(PatrolTimer);
	PatrolTarget = HearnPawn;
	
	if (EnemyState != EEnemyState::EES_Attacking)
	{
		EnemyState = EEnemyState::EES_Chasing;
		FVector TargetLocation = PatrolTarget->GetActorLocation(); // 플레이어의 위치를 복사하여 전달
		MoveToLocation(TargetLocation); // 플레이어 위치로 이동
	}
}

void AEnemyBaseCharacter::Elim()
{
	Super::Elim();
}

void AEnemyBaseCharacter::SpawnWeaponActor()
{
	if(!HasAuthority()) return;
	if (WeaponActorClass == nullptr) return;
	
	UWorld* World = GetWorld();
	if (World == nullptr) return;
	
	FVector Location = GetActorLocation();
	FRotator Rotation = GetActorRotation();
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this; // Setting the owner

	const TObjectPtr<AWeapon> EquippedWeapon =
		World->SpawnActor<AWeapon>(WeaponActorClass, Location, Rotation, SpawnParams);

	if (!EquippedWeapon) return;
	
	UE_LOG(LogTemp, Warning, TEXT("Spawned Weapon Actor: %s"), *EquippedWeapon->GetName());
	CombatComp->EquipWeapon(EquippedWeapon);

	AttackRadius = Cast<ARangeWeapon>(EquippedWeapon) ? 1000 : 150;
	
}