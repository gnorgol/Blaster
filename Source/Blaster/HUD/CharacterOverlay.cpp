// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterOverlay.h"
#include "Components/TextBlock.h"
#include "Components/EditableTextBox.h"
#include "Blaster/Weapon/Weapon.h"
#include "Blaster/Weapon/WeaponTypes.h"
#include "Components/RichTextBlock.h"
#include "Components/ScrollBox.h"

void UCharacterOverlay::SetKillFeedText(FText AttackerName, FText VictimName, FText PlayerName, EWeaponType WeaponTypesKiller)
{
	FString KillText = KillFeedText->GetText().ToString();
	UE_LOG(LogTemp, Warning, TEXT("AttackerName: %s"), *AttackerName.ToString());
	UE_LOG(LogTemp, Warning, TEXT("VictimName: %s"), *VictimName.ToString());
	UE_LOG(LogTemp, Warning, TEXT("PlayerName: %s"), *PlayerName.ToString());


	if (PlayerName.ToString() != AttackerName.ToString())
	{
		KillText.Append("<Red>" + AttackerName.ToString() + "</>");
	}
	else
	{
		KillText.Append("<Green>" + AttackerName.ToString() + "</>");
	}
	switch (WeaponTypesKiller)
	{
	case EWeaponType::EWT_AssaultRifle:
		KillText.Append(" <img id=\"AssaultRifle\"/> ");
		break;
	case EWeaponType::EWT_Shotgun:
		KillText.Append(" <img id=\"AssaultRifle\"/> ");
		break;
	case EWeaponType::EWT_SniperRifle:
		KillText.Append(" <img id=\"AssaultRifle\"/> ");
		break;
	case EWeaponType::EWT_Pistol:
		KillText.Append(" <img id=\"Pistol\"/> ");
		break;
	case EWeaponType::EWT_RocketLauncher:
		KillText.Append(" <img id=\"AssaultRifle\"/> ");
		break;
	case EWeaponType::EWT_SubmachineGun:
		KillText.Append(" <img id=\"AssaultRifle\"/> ");
		break;
	case EWeaponType::EWT_GrenadeLauncher:
		KillText.Append(" <img id=\"AssaultRifle\"/> ");
		break;
	case EWeaponType::EWT_Grenade:
		KillText.Append(" <img id=\"AssaultRifle\"/> ");
		break;
	case EWeaponType::EWT_MAX:
		KillText.Append(" <img id=\"AssaultRifle\"/> ");
		break;
	default:
		break;
	}
	if (AttackerName.ToString() != VictimName.ToString() && VictimName.ToString() != PlayerName.ToString())
	{
		//KillText.Append(VictimName.ToString());
		KillText.Append("<Red>" + VictimName.ToString() + "</>");
	}
	else if (VictimName.ToString() == PlayerName.ToString())
	{
		KillText.Append("<Green>" + VictimName.ToString() + "</>");
	}
	KillText.Append("\n");
	KillFeedText->SetText(FText::FromString(KillText));
	KillFeedScrollBox->ScrollToEnd();

}
