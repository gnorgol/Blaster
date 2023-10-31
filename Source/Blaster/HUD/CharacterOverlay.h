// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterOverlay.generated.h"

/**
 * 
 */
class UProgressBar;
class UTextBlock;
class UMultiLineEditableTextBox;
UCLASS()
class BLASTER_API UCharacterOverlay : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget))
		UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
		UTextBlock* HealthText;
	UPROPERTY(meta = (BindWidget))
		UTextBlock* ScoreAmountText;
	UPROPERTY(meta = (BindWidget))
		UTextBlock* DefeatsAmountText;
	UPROPERTY(meta = (BindWidget))
		UTextBlock* KillFieldPlayerText;
	UPROPERTY(meta = (BindWidget))
		UMultiLineEditableTextBox* KillFieldTextBox;
	UPROPERTY(meta = (BindWidget))
		UTextBlock* WeaponAmmoAmountText;
	UPROPERTY(meta = (BindWidget))
		UTextBlock* CarriedAmmoAmountText;
	UPROPERTY(meta = (BindWidget))
		UTextBlock* MatchCountdownText;
};
