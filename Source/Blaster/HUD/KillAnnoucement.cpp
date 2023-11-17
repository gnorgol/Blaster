// Fill out your copyright notice in the Description page of Project Settings.


#include "KillAnnoucement.h"
#include "Blaster/Weapon/Weapon.h"
#include "Blaster/Weapon/WeaponTypes.h"
#include "Components/RichTextBlock.h"


void UKillAnnoucement::SetKillAnnoucementText(FText AttackerName, FText VictimName, AWeapon* WeaponKiller)
{
	EWeaponType WeaponType = WeaponKiller->GetWeaponType();
	FString KillText = "";
	KillText.Append(AttackerName.ToString());
	//<img id="Pistol"/> 
	switch (WeaponType)
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
	case EWeaponType::EWT_MAX:
		KillText.Append(" <img id=\"AssaultRifle\"/> ");
		break;
	default:

		break;
	}
	if (AttackerName.ToString() != VictimName.ToString())
	{
		KillText.Append(VictimName.ToString());
	}
	if (KillFieldText)
	{
		KillFieldText->SetText(FText::FromString(KillText));
	}


}
