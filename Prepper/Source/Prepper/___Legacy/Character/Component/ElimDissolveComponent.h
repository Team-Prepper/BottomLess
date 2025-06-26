// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "Prepper/___Legacy/Character/Component/CharacterComponent.h"
#include "ElimDissolveComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PREPPER_API UElimDissolveComponent : public UCharacterComponent
{
	GENERATED_BODY()
	
private:
	UPROPERTY()
	TObjectPtr<ACharacter> TargetCharacter;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UTimelineComponent> DissolveTimeline;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UCurveFloat> DissolveCurve;
	
	UPROPERTY(VisibleAnywhere, Category = Elim)
	TObjectPtr<UMaterialInstanceDynamic> DynamicDissolveMaterialInstance;
	// 블루 프린트에 세팅
	UPROPERTY(EditAnywhere, Category = Elim)
	TObjectPtr<UMaterialInstance> DissolveMaterialInstance;
	
	FOnTimelineFloat DissolveTrack;
	// DissolveMaterialInstance로 부터 동적 생성

	UPROPERTY(EditAnywhere)
	float DestroyDelayTime = 5.f;

public:
	// Sets default values for this pawn's properties
	UElimDissolveComponent();
	
	virtual void SetCharacter(ABaseCharacter* Target) override;
	void SetTargetCharacter(TObjectPtr<ACharacter> Target);

	virtual void TargetElim() override;

	UFUNCTION()
	void UpdateDissolveMaterial(const float DissolveValue);
	UFUNCTION()
	void RemoveCharacter();
};
