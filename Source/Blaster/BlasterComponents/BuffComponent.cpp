// Fill out your copyright notice in the Description page of Project Settings.


#include "BuffComponent.h"
#include "Blaster/Character/BlasterCharacter.h"


UBuffComponent::UBuffComponent()
{

	PrimaryComponentTick.bCanEverTick = true;

}

void UBuffComponent::Heal(float HealAmount, float HealTime)
{
	if (Character == nullptr || Character->IsDead())
	{
		return;
	}
	if (HealTime == 0)
	{
		AmountToHeal = FMath::Clamp(Character->GetHealth() + HealAmount, 0.f, Character->GetMaxHealth());
		Character->SetHealth(AmountToHeal);
		Character->UpdateHUDHealth();
		AmountToHeal = 0.f;
		return;
	}
	bHealing = true;
	HealRate = HealAmount / HealTime;
	AmountToHeal = HealAmount;
}

void UBuffComponent::Shield(float ShieldAmount, float ShieldTime)
{
	if (Character == nullptr || Character->IsDead())
	{
		return;
	}
	bShielding = true;
	if (ShieldTime == 0)
	{
		Character->SetShield(Character->GetShield() + ShieldAmount);
		Character->UpdateHUDShield();
		return;

	}
	ShieldRate = ShieldAmount / ShieldTime;
	AmountToShield = ShieldAmount - 1.f;
}



void UBuffComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UBuffComponent::HealRampUp(float DeltaTime)
{
	if (!bHealing || Character == nullptr || Character->IsDead())
	{
		return;
	}
	const float HealThisFrame = HealRate * DeltaTime;
	Character->SetHealth(FMath::Clamp(Character->GetHealth() + HealThisFrame, 0.f, Character->GetMaxHealth()));
	Character->UpdateHUDHealth();
	AmountToHeal -= HealThisFrame;

	if (AmountToHeal <= 0.f || Character->GetHealth() >= Character->GetMaxHealth())
	{
		bHealing = false;
		AmountToHeal = 0.f;
	}

}

void UBuffComponent::ShieldRampUp(float DeltaTime)
{
	if (!bShielding || Character == nullptr || Character->IsDead())
	{
		return;
	}
	const float ShieldThisFrame = ShieldRate * DeltaTime;
	Character->SetShield(FMath::Clamp(Character->GetShield() + ShieldThisFrame, 0.f, Character->GetMaxShield()));
	Character->UpdateHUDShield();
	AmountToShield -= ShieldThisFrame;
	if (AmountToShield <= 0.f || Character->GetShield() >= Character->GetMaxShield())
	{
		bShielding = false;
		AmountToShield = 0.f;
	}
}


void UBuffComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (bHealing)
	{
		HealRampUp(DeltaTime);
	}
	if (bShielding)
	{
		ShieldRampUp(DeltaTime);
	}
}

