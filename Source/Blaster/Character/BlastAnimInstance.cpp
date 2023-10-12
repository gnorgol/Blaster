// Fill out your copyright notice in the Description page of Project Settings.


#include "BlastAnimInstance.h"
#include "BlasterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UBlastAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
}

void UBlastAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (BlasterCharacter == nullptr)
	{
		BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
	}
	if (BlasterCharacter == nullptr)
	{
		return;
	}
	FVector Velocity = BlasterCharacter->GetVelocity();
	Velocity.Z = 0.0f;
	Speed = Velocity.Size();

	bIsInAir = BlasterCharacter->GetCharacterMovement()->IsFalling();
	bIsAccelerating = BlasterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.0f ? true : false;
	bWeaponEquipped = BlasterCharacter->IsWeaponEquipped();
	bIsCrouching = BlasterCharacter->GetCharacterMovement()->IsCrouching();
	bIsAiming = BlasterCharacter->IsAiming();

	//Calculate Yaw Offset for Strafing
	FRotator AimRotation = BlasterCharacter->GetBaseAimRotation();
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(BlasterCharacter->GetVelocity());
	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation,AimRotation );
	DeltaRot = FMath::RInterpTo(DeltaRotation,DeltaRot,DeltaTime,6.f);
	YawOffset = DeltaRot.Yaw;

	
	CharacterRotationLastFrame = CharacterRotation;
	CharacterRotation = BlasterCharacter->GetActorRotation();
	const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
	const float Target = Delta.Yaw / DeltaTime;
	const float Interp = FMath::FInterpTo(Lean, Target, DeltaTime, 6.0f);
	Lean = FMath::Clamp(Interp, -90.0f, 90.0f);

}
