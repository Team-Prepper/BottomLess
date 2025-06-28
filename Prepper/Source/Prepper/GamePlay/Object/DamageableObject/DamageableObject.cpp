// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageableObject.h"

#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Prepper/GamePlay/HealthPointComponent/HealthPointComponent.h"
#include "Sound/SoundCue.h"

ADamageableObject::ADamageableObject()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	ObjectMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ObjectMesh"));
	SetRootComponent(ObjectMesh);
	ObjectMesh->SetCollisionObjectType(ECC_WorldStatic);
	ObjectMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	
	HealthPoint = CreateDefaultSubobject<UHealthPointComponent>(TEXT("HealthPointComponent"));
}

void ADamageableObject::BeginPlay()
{
	Super::BeginPlay();
	if(HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &IDamageable::DynamicDamage);
	}
}

void ADamageableObject::RemoveAction()
{
	if(DestroyParticles)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			DestroyParticles,
			GetActorLocation()
		);
	}
	if(DestroySound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DestroySound, GetActorLocation());
	}
}

void ADamageableObject::ReceiveDamage(float Damage, AController* InstigatorController, AActor* DamageCauser)
{
	HealthPoint->TakeDamage(Damage);
	
	if (HealthPoint->GetCurHealth() > 0) return;

	Destroy(true);
}

void ADamageableObject::Destroyed()
{
	Super::Destroyed();
	RemoveAction();
}
