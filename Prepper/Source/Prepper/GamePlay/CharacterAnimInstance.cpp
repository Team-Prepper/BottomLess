// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterAnimInstance.h"
// Fill out your copyright notice in the Description page of Project Settings.


#include "BCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	PlayerCharacter = Cast<ABCharacter>(TryGetPawnOwner());
}

void UCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if(PlayerCharacter == nullptr)
	{
		PlayerCharacter = Cast<ABCharacter>(TryGetPawnOwner());
	}
	
	if(PlayerCharacter == nullptr) return;

	FVector Velocity = PlayerCharacter->GetVelocity();
	Velocity.Z = 0.f;
	
	Speed = Velocity.Size();

	bIsInAir = PlayerCharacter->GetCharacterMovement()->IsFalling();

	bIsAccelerating = PlayerCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f;
	bWeaponEquipped = false;
	bIsCrouch = PlayerCharacter->bIsCrouched;
	bAiming = PlayerCharacter->GetIsAiming();

}