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
	bHealing = true;
	HealRate = HealAmount / HealTime;
	AmountToHeal = HealAmount;
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

	if (AmountToHeal <= 0.f)
	{
		bHealing = false;
		AmountToHeal = 0.f;
	}

}


void UBuffComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	HealRampUp(DeltaTime);

}

