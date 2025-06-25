// Fill out your copyright notice in the Description page of Project Settings.


#include "ElimDissolveComponent.h"

#include "Components/CapsuleComponent.h"
#include "Prepper/___Legacy/Character/BaseCharacter.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values
UElimDissolveComponent::UElimDissolveComponent()
{
	DissolveTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("DissolveTimelineComponent"));
}

void UElimDissolveComponent::TargetElim(const bool IsNetworkElim)
{
	if (IsNetworkElim)
	{
		MulticastTargetElim();
		return;
	}

	// Disable Movement
	TargetCharacter->GetCharacterMovement()->DisableMovement();
	TargetCharacter->GetCharacterMovement()->StopMovementImmediately();
	
	// Disable Collision
	TargetCharacter->SetActorEnableCollision(false);
	TargetCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TargetCharacter->GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	// Start Dissolve Effect
	if (DissolveMaterialInstance)
	{
		DynamicDissolveMaterialInstance = UMaterialInstanceDynamic::Create(DissolveMaterialInstance, this);

		TargetCharacter->GetMesh()->SetMaterial(0, DynamicDissolveMaterialInstance);

		TArray<USceneComponent*> AttachedComponents;
		TargetCharacter->GetMesh()->GetChildrenComponents(true,  AttachedComponents);

		// Loop through all found Static Mesh components
		for (USceneComponent* SceneComponent : AttachedComponents)
		{
			if(UStaticMeshComponent* SMComp = Cast<UStaticMeshComponent>(SceneComponent))
			{
				SMComp->SetMaterial(0, DynamicDissolveMaterialInstance);
			}
		}
			
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("DissolveValue"), 0.55f);
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("GlowValue"), 200.f);
	}
	
	DissolveTrack.BindDynamic(this, &UElimDissolveComponent::UpdateDissolveMaterial);
	
	if (DissolveCurve && DissolveTimeline)
	{
		DissolveTimeline->AddInterpFloat(DissolveCurve, DissolveTrack);
		DissolveTimeline->Play();
	}
	
	if (TargetCharacter)
	{
		FTimerHandle TimerHandle;
		const FTimerDelegate TimerDelegate;
        
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, DestroyDelayTime, false);
	}
}

void UElimDissolveComponent::MulticastTargetElim_Implementation()
{
	TargetElim(false);
}

void UElimDissolveComponent::RemoveCharacter()
{
	TargetCharacter->Destroy();
}

void UElimDissolveComponent::SetCharacter(ABaseCharacter* Target)
{
	SetTargetCharacter(Target);
}

void UElimDissolveComponent::SetTargetCharacter(const TObjectPtr<ACharacter> Target)
{
	TargetCharacter = Target;
}

void UElimDissolveComponent::TargetElim()
{
	TargetElim(false);
}

void UElimDissolveComponent::UpdateDissolveMaterial(const float DissolveValue)
{
	if (DynamicDissolveMaterialInstance)
	{
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("DissolveValue"), DissolveValue);
	}
}
