// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageableObject.h"

#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Sound/SoundCue.h"

ADamageableObject::ADamageableObject()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	ObjectMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ObjectMesh"));
	SetRootComponent(ObjectMesh);
	ObjectMesh->SetCollisionObjectType(ECC_WorldStatic);
	ObjectMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
}

void ADamageableObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADamageableObject, CurrentHealth);
}

void ADamageableObject::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth;

	if(HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &IDamageable::DynamicDamage);
	}
}

void ADamageableObject::ReceiveDamage(float Damage, AController* InstigatorController, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("%f"), Damage)
	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.f, MaxHealth);
	if(CurrentHealth <= 0)
	{
		ServerDestroyObject();
	}
}

void ADamageableObject::OnRep_Health()
{
	// Effect
	if(ImpactParticles)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			ImpactParticles,
			GetActorLocation()
		);
	}
	if(ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}
}

void ADamageableObject::RemoveAction()
{
	
}

void ADamageableObject::ServerDestroyObject_Implementation()
{
	MulticastDestroyObject();
}

void ADamageableObject::MulticastDestroyObject_Implementation()
{
	RemoveAction();
	Destroy();
}

void ADamageableObject::Destroyed()
{
	Super::Destroyed();

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
