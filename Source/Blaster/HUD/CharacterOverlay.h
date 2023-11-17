// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <Components/ScrollBox.h>
#include <Blaster/Weapon/WeaponTypes.h>
#include "CharacterOverlay.generated.h"



/**
 * 
 */
class UProgressBar;
class UTextBlock;
class UMultiLineEditableTextBox;
class UImage;
class URichTextBlock;
class UHorizontalBox;
class UKillFeedScrollBox;
class AWeapon;
UCLASS()
class BLASTER_API UCharacterOverlay : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget))
		UProgressBar* HealthBar;
	UPROPERTY(meta = (BindWidget))
		UProgressBar* ShieldBar;

	UPROPERTY(meta = (BindWidget))
		UTextBlock* HealthText;
	UPROPERTY(meta = (BindWidget))
		UTextBlock* ShieldText;
	UPROPERTY(meta = (BindWidget))
		UTextBlock* ScoreAmountText;
	UPROPERTY(meta = (BindWidget))
		UTextBlock* DefeatsAmountText;
	UPROPERTY(meta = (BindWidget))
		UTextBlock* KillFieldPlayerText;
	UPROPERTY(meta = (BindWidget))
		UTextBlock* WeaponAmmoAmountText;
	UPROPERTY(meta = (BindWidget))
		UTextBlock* CarriedAmmoAmountText;
	UPROPERTY(meta = (BindWidget))
		UTextBlock* MatchCountdownText;
	UPROPERTY(meta = (BindWidget))
		UTextBlock* GrenadeAmountText;
	UPROPERTY(meta = (BindWidget))
		UImage* HighPingImage;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
		UWidgetAnimation* HighPingAnimation;
	UPROPERTY(meta = (BindWidget))
		UScrollBox* KillFeedScrollBox;
	UPROPERTY(meta = (BindWidget))
		URichTextBlock* KillFeedText;

	void SetKillFeedText(FText AttackerName, FText VictimName, FText PlayerName, EWeaponType WeaponTypesKiller);
};
