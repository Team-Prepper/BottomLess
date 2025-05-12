#pragma once

#include "CoreMinimal.h"
#include "Prepper/_Base/ObserverPattern/Subject.h"
#include "Prepper/_Base/Util/GaugeValue.h"
#include "GameFramework/Character.h"
#include "Prepper/GamePlay/Damageable.h"
#include "BaseCharacter.generated.h"

class UCharacterComponent;
class UElimDissolveComponent;
class UBaseCombatComponent;

UENUM(BlueprintType)
enum class EMovementState : uint8
{
	EMS_Seat UMETA(DisplayName = "Seat"),
	EMS_Aim UMETA(DisplayName = "Aim"),
	EMS_Sprint UMETA(DisplayName = "Sprint"),
	EMS_Idle UMETA(DisplayName = "Idle"),
    
	EMS_MAX UMETA(DisplayName = "Default Max")
};

UCLASS()
class PREPPER_API ABaseCharacter : public ACharacter, public IDamageable, public ISubject<GaugeValue<float>>
{
	GENERATED_BODY()
// Actor
public:
	ABaseCharacter();
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void PostInitializeComponents() override;
	// 네트워크 동기화 변수 설정
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	TArray<TObjectPtr<UCharacterComponent>> CharacterComponents;
	
private:
	TSet<IObserver<GaugeValue<float>>*> Observers;
public:
	virtual void Attach(IObserver<GaugeValue<float>>* Observer) override;
	virtual void Detach(IObserver<GaugeValue<float>>* Observer) override;
	virtual void Notify();
protected:
	/* 기본 변수 */
	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxHealth = 100.f;
	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere, Category = "Player Stats")
	float CurrentHealth = 100.f;
	
	/* 데미지 처리 */
	UPROPERTY(EditAnywhere, Category = Combat)
	TObjectPtr<UAnimMontage> HitReactMontage;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UElimDissolveComponent> ElimEvent;
	UPROPERTY(EditAnywhere, Category = Combat)
	TObjectPtr<UAnimMontage> ElimMontage;

	TSet<TObjectPtr<AActor>> AttachedActor;
	
public:
	virtual void Elim();
	
	virtual void MulticastElimAction() {}
	UFUNCTION(NetMulticast, Reliable)
	void MulticastElim();
	
	FORCEINLINE bool IsElimed() const { return CurrentHealth <= 0; }
	
	void PlayAnim(UAnimMontage* Montage, const FName& SectionName = "") const;
	void AttachActorAtSocket(const FName& SocketName, AActor* TargetActor);
	void SetAttachedHidden(const bool Visible);
	
	UFUNCTION()
	virtual void ReceiveDamage(float Damage, AController* InstigatorController, AActor* DamageCauser) override;
	
// Movement
protected:
	UPROPERTY(EditAnywhere, Category = "Player Movement Speed")
	float WalkSpeed = 600;
	UPROPERTY(EditAnywhere, Category = "Player Movement Speed")
	float SprintSpeed = 900;
	UPROPERTY(EditAnywhere, Category = "Player Movement Speed")
	float AimMovementSpeed = 400.f;
	
	UPROPERTY(Replicated)
	EMovementState MovementState;
	
public:
	float CoefficientMovementSpeed = 1;
	void SetMovementState(const EMovementState& State);
	FORCEINLINE EMovementState GetMovementState() const { return MovementState; }
	
protected:
	UPROPERTY()
	class ADeathMatchPlayerState* DeathMatchPlayerState;
	
	void ConvertMovementState(const EMovementState& State);
	UFUNCTION(Server, Reliable)
	void ServerConvertMovementState(const EMovementState& State);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastConvertMovementState(const EMovementState& State);

	virtual void SeatToggle(bool Seat);
	
	virtual void PlayHitReactMontage();
	UFUNCTION()
	virtual void OnRep_Health();
// Attack
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UBaseCombatComponent> CombatComp;
public:
	virtual void AttackTrigger(const bool IsTrigger);
	virtual void Reload();
	virtual void AimTrigger(const bool IsTrigger);
};