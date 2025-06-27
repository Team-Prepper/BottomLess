#include "BaseCharacter.h"

#include "Component/Combat/BaseCombatComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Net/UnrealNetwork.h"
#include "Prepper/GamePlay/PrepperGameMode.h"
#include "Prepper/GamePlay/Character/Component/ElimDissolveComponent.h"
#include "Prepper/___Legacy/PlayerController/BasePlayerController.h"
#include "Prepper/__Base/Util/GaugeFloat.h"

// Actor
ABaseCharacter::ABaseCharacter()
{
	WalkSpeed = 600.f;
	SprintSpeed = 900.f;
	
	ElimEvent = CreateDefaultSubobject<UElimDissolveComponent>(TEXT("ElimEventComponent"));

	ElimEvent->SetIsReplicated(true);
	
	MovementState = EMovementState::EMS_Idle;
}

void ABaseCharacter::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
}

void ABaseCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	GetComponents<TObjectPtr<UCharacterComponent>>(CharacterComponents);
	
	for (int i = 0; i < CharacterComponents.Num(); i++)
	{
		CharacterComponents[i]->SetCharacter(this);
	}
}

void ABaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABaseCharacter, CurrentHealth);
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	MovementState = EMovementState::EMS_Idle;
	ElimEvent->SetCharacter(this);
	if (HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &IDamageable::DynamicDamage);
	}
}

// Observer Pattern
void ABaseCharacter::Attach(IObserver<GaugeValue<float>>* Observer)
{
	Observers.Add(Observer);;
	UE_LOG(LogTemp, Warning, TEXT("CurrentHealth: %f"), CurrentHealth);
	Observer->Update(FGaugeFloat(CurrentHealth, MaxHealth));
}

void ABaseCharacter::Detach(IObserver<GaugeValue<float>>* Observer)
{
	Observers.Remove(Observer);
}

void ABaseCharacter::Notify()
{
	const FGaugeFloat Value(FGaugeFloat(CurrentHealth, MaxHealth));
	for (const auto Observer : Observers)
	{
		Observer->Update(Value);
	}
}

// Character
void ABaseCharacter::AttachActorAtSocket(const FName& SocketName, AActor* TargetActor)
{
	const USkeletalMeshSocket* TargetSocket = GetMesh()->GetSocketByName(SocketName);
	if(TargetSocket)
	{
		AttachedActor.Add(TargetActor);
		TargetSocket->AttachActor(TargetActor, GetMesh());
	}
	UE_LOG(LogTemp, Warning, TEXT("Attach %s"), *SocketName.ToString());
}

void ABaseCharacter::SetAttachedHidden(const bool Visible)
{
	for(TObjectPtr<AActor> TargetActor : AttachedActor)
	{
		TargetActor->SetActorHiddenInGame(Visible);
	}
}

void ABaseCharacter::PlayAnim(UAnimMontage* Montage, const FName& SectionName) const
{
	if (Montage == nullptr) return;
	
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance) return;
	
	AnimInstance->Montage_Play(Montage);
	if (SectionName.Compare("") == 0) return;
	
	AnimInstance->Montage_JumpToSection(SectionName);
}

// Health
void ABaseCharacter::ReceiveDamage(float Damage, AController* InstigatorController, AActor* DamageCauser)
{
	CurrentHealth -= Damage;
	PlayHitReactMontage();

	if (CurrentHealth < 0) CurrentHealth = 0;
	if(CurrentHealth != 0.f) return;
	
	// 해당 캐릭터가 사망했다면 
	APrepperGameMode* PrepperGameMode =  GetWorld()->GetAuthGameMode<APrepperGameMode>();
	
	if(PrepperGameMode == nullptr) return;
	
	ABasePlayerController* PrepperPlayerController = Cast<ABasePlayerController>(Controller);
	ABasePlayerController* AttackerController = Cast<ABasePlayerController>(InstigatorController);
	
	PrepperGameMode->PlayerEliminated(this, PrepperPlayerController, AttackerController);
}

void ABaseCharacter::SeatToggle(bool Seat)
{
}

void ABaseCharacter::PlayHitReactMontage()
{
	PlayAnim(HitReactMontage, FName("HitFront"));
}

void ABaseCharacter::OnRep_Health()
{
	PlayHitReactMontage();
	Notify();
}

void ABaseCharacter::AttackTrigger(const bool IsTrigger)
{
	if (CombatComp == nullptr) return;
	CombatComp->FireTrigger(IsTrigger);
}

void ABaseCharacter::Reload()
{
	if (CombatComp == nullptr) return;
	CombatComp->Reload();
}

void ABaseCharacter::AimTrigger(const bool IsTrigger)
{
	if (CombatComp == nullptr) return;
	CombatComp->SetAiming(IsTrigger);
}

void ABaseCharacter::Elim()
{
	AttachedActor.Empty();
	
	MulticastElim();
}

void ABaseCharacter::MulticastElim_Implementation()
{
	MulticastElimAction();
	
	for (int i = 0; i < CharacterComponents.Num(); i++)
	{
		CharacterComponents[i]->TargetElim();
	}
	
	PlayAnim(ElimMontage);
	ElimEvent->TargetElim();

	// Disable Movement
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->StopMovementImmediately();
	
	// Disable Collision
	SetActorEnableCollision(false);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
}

void ABaseCharacter::SetMovementState(const EMovementState& State)
{
	if(!IsLocallyControlled() && !HasAuthority()) return;
	ServerConvertMovementState(State);
}

void ABaseCharacter::ServerConvertMovementState_Implementation(const EMovementState& State)
{
	MulticastConvertMovementState(State);
}

void ABaseCharacter::MulticastConvertMovementState_Implementation(const EMovementState& State)
{
	ConvertMovementState(State);
}

void ABaseCharacter::ConvertMovementState(const EMovementState& State)
{
	if(MovementState == EMovementState::EMS_Seat)
	{
		SeatToggle(false);
	}
	
	MovementState = State;
	
	switch (MovementState)
	{
	case EMovementState::EMS_Seat:
		SeatToggle(true);
		break;
	case EMovementState::EMS_Aim:
		GetCharacterMovement()->MaxWalkSpeed = AimMovementSpeed * CoefficientMovementSpeed;
		break;
	case EMovementState::EMS_Sprint:
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed * CoefficientMovementSpeed;
		break;
	case EMovementState::EMS_Idle:
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed * CoefficientMovementSpeed;
		break;
	default:
		break;
	}
}