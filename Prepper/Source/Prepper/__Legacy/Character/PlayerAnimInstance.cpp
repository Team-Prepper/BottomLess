// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnimInstance.h"
#include "PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Prepper/__Legacy/Weapon/RangeWeapon/RangeWeapon.h"

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	PlayerCharacter = Cast<APlayerCharacter>(TryGetPawnOwner());
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if(PlayerCharacter == nullptr)
	{
		PlayerCharacter = Cast<APlayerCharacter>(TryGetPawnOwner());
	}
	if(PlayerCharacter == nullptr) return;

	FVector Velocity = PlayerCharacter->GetVelocity();
	Velocity.Z = 0.f;
	Speed = Velocity.Size();

	bIsInAir = PlayerCharacter->GetCharacterMovement()->IsFalling();

	bIsAccelerating = PlayerCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f;
	bWeaponEquipped = PlayerCharacter->IsWeaponEquipped();
	EquippedWeapon = PlayerCharacter->GetEquippedWeapon();
	bIsCrouch = PlayerCharacter->bIsCrouched;
	bAiming = PlayerCharacter->IsAiming();
	TurningInPlace = PlayerCharacter->GetTurningInPlace();
	
	bRotateRootBone = PlayerCharacter->ShouldRotateRootBone();
	bElimmed = PlayerCharacter->IsElimed();
	bUseAimOffsets = PlayerCharacter->GetCombatState() != ECombatState::ECS_Reloading && !PlayerCharacter->GetDisableGamePlay();
	bTransformRightHand = PlayerCharacter->GetCombatState() != ECombatState::ECS_Reloading && !PlayerCharacter->GetDisableGamePlay();
	bEquippedMiniGun = PlayerCharacter->GetEquippedWeapon() != nullptr ? PlayerCharacter->GetEquippedWeapon()->GetWeaponType() == EWeaponType::EWT_MiniGun : false;
	bEquippedMeleeWeapon = PlayerCharacter->GetEquippedWeapon() != nullptr ? (PlayerCharacter->GetEquippedWeapon()->GetWeaponType() == EWeaponType::EWT_MeleeWeaponBlunt ||
		PlayerCharacter->GetEquippedWeapon()->GetWeaponType() == EWeaponType::EWT_MeleeWeaponSword) : false;

	bUseFABRIK = PlayerCharacter->GetCombatState() < ECombatState::ECS_Reloading;
	if(PlayerCharacter->IsLocallyControlled())
	{
		bUseFABRIK = !PlayerCharacter->IsLocallyReloading();
		if(bEquippedMeleeWeapon)
		{
			bUseFABRIK = !bEquippedMeleeWeapon;
		}
	}
	
	// OFFSET YAW FOR STRAFING
	FRotator AimRotation = PlayerCharacter->GetBaseAimRotation();
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(PlayerCharacter->GetVelocity());
	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation,	AimRotation);
	DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRot, DeltaSeconds, 6.f);
	YawOffset = DeltaRotation.Yaw;

	CharacterRotationLastFrame = CharacterRotation;
	CharacterRotation = PlayerCharacter->GetActorRotation();
	const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
	const float Target = Delta.Yaw / DeltaSeconds;
	const float Interp = FMath::FInterpTo(Lean, Target, DeltaSeconds, 6.f);
	Lean = FMath::Clamp(Interp, -90.f,90.f);

	AO_Yaw = PlayerCharacter->GetAO_Yaw();
	AO_Pitch = PlayerCharacter->GetAO_Pitch();
	
	YawOffset = 45;
	AO_Yaw = 45;
	AO_Pitch = 45;
	Lean = 45;

	if(bWeaponEquipped && EquippedWeapon && EquippedWeapon->GetWeaponMesh() && PlayerCharacter->GetMesh())
	{
		if(Cast<ARangeWeapon>(EquippedWeapon))
		{
			LeftHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"), RTS_World);
			FVector OutPosition;
			FRotator OutRotator;
			PlayerCharacter->GetMesh()->TransformToBoneSpace(FName("hand_r"), LeftHandTransform.GetLocation(), FRotator::ZeroRotator, OutPosition, OutRotator);
			LeftHandTransform.SetLocation(OutPosition);
			LeftHandTransform.SetRotation(FQuat(OutRotator));

			if(PlayerCharacter->IsLocallyControlled())
			{
				bLocallyControlled = true;
				FTransform RightHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("Hand_R"), RTS_World);
				FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(RightHandTransform.GetLocation(), RightHandTransform.GetLocation() + (RightHandTransform.GetLocation() - PlayerCharacter->GetHitTarget()));
				RightHandRotation = FMath::RInterpTo(RightHandRotation, LookAtRotation, DeltaSeconds, 30.f);
			}
		}
	}

}

