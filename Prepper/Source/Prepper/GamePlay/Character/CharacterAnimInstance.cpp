// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterAnimInstance.h"
// Fill out your copyright notice in the Description page of Project Settings.


#include "BCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Prepper/__Legacy/Character/Enums/TurningInPlace.h"

void UCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	PlayerCharacter = Cast<ABCharacter>(TryGetPawnOwner());
	bWeaponEquipped = false;
	bRotateRootBone = true;
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
	TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	
	bElimmed = false;
	bUseAimOffsets = true;
	bTransformRightHand = false;
	
	bUseFABRIK = true;
	
	if (!bRotateRootBone && Speed > 0)
	{
		// OFFSET YAW FOR STRAFING
		const FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(
			UKismetMathLibrary::MakeRotFromX(Velocity), PlayerCharacter->GetBaseAimRotation());

		AO_Yaw = 0;
		YawOffset = DeltaRot.Yaw;
		AO_Pitch = DeltaRot.Pitch;

	}

	UE_LOG(LogTemp, Warning, TEXT("%f"), PlayerCharacter->GetBaseAimRotation().Yaw);
	
	if (!bWeaponEquipped) return;
	if (!PlayerCharacter->GetMesh()) return;

	return;
	/*
	LeftHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"), RTS_World);
	FVector OutPosition;
	FRotator OutRotator;
	PlayerCharacter->GetMesh()->TransformToBoneSpace(FName("hand_r"),LeftHandTransform.GetLocation(),
		FRotator::ZeroRotator, OutPosition, OutRotator);
	LeftHandTransform.SetLocation(OutPosition);
	LeftHandTransform.SetRotation(FQuat(OutRotator));
	*/

}

void UCharacterAnimInstance::SetEquippedWeaponType(const EWeaponType WeaponType)
{
	bWeaponEquipped = WeaponType != EWeaponType::EWT_MAX;
	
	bRotateRootBone = !bWeaponEquipped;
	bEquippedMiniGun = WeaponType == EWeaponType::EWT_MiniGun;
	bEquippedMeleeWeapon = WeaponType == EWeaponType::EWT_MeleeWeaponBlunt ||
		WeaponType == EWeaponType::EWT_MeleeWeaponSword;

}

void UCharacterAnimInstance::SetAiming(const bool Trigger)
{
	bAiming = Trigger;
}

void UCharacterAnimInstance::SetCrouch(const bool Crouch)
{
	bIsCrouch = Crouch;
}
