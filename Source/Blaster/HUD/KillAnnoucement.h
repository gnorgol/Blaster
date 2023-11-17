// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blaster/Weapon/Weapon.h"
#include "KillAnnoucement.generated.h"

/**
 * 
 */
class URichTextBlock;
class UHorizontalBox;
UCLASS()
class BLASTER_API UKillAnnoucement : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
		UHorizontalBox* KillFieldBox;
	UPROPERTY(meta = (BindWidget))
		URichTextBlock* KillFieldText;

	void SetKillAnnoucementText(FText AttackerName, FText VictimName, AWeapon* WeaponKiller);

	
};
