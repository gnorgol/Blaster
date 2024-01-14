// Fill out your copyright notice in the Description page of Project Settings.


#include "OverheadWidget.h"
#include "Components/TextBlock.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerState.h"


void UOverheadWidget::SetDisplayText(FString TextToDisplay)
{
	if (DisplayText)
	{
		DisplayText->SetText(FText::FromString(TextToDisplay));
	}
}

void UOverheadWidget::SetColor(FLinearColor Color)
{
	if (DisplayText)
	{
		DisplayText->SetColorAndOpacity(Color);
	}
}

void UOverheadWidget::ShowPlayerNetRole(APawn* InPawn)
{

	ENetRole RemoteRole = InPawn->GetRemoteRole();
	FString Role;
	switch (RemoteRole)
	{
	case ENetRole::ROLE_Authority:
		Role = FString("Authority");
		break;
	case ENetRole::ROLE_AutonomousProxy:
		Role = FString("Autonomous Proxy");
		break;
	case ENetRole::ROLE_SimulatedProxy:
		Role = FString("Simulated Proxy");
		break;
	case ENetRole::ROLE_None:
		Role = FString("None");
		break;
	}
	FString RemoteRoleString = FString::Printf(TEXT("Remote Role: %s"), *Role);
	SetDisplayText(RemoteRoleString);

}

void UOverheadWidget::ShowPlayerName(APawn* InPawn, FLinearColor TextColor)
{
	APlayerState* PlayerState = InPawn->GetPlayerState<APlayerState>();
	
	if (PlayerState && !InPawn->IsLocallyControlled())
	{
		//add text color red 

		FString PlayerName = PlayerState->GetPlayerName();
		//FString PlayerNetID = FString::Printf(TEXT("Net ID: %d"), PlayerState->GetPlayerId());
		//FString PlayerUniqueNetID = PlayerState->GetUniqueId().ToString();
		SetDisplayText(PlayerName);
		SetColor(TextColor);

	}
	//if the player is too far away set the text to empty
	ACharacter* LocalPawn = GetOwningPlayerPawn<ACharacter>();
	if (LocalPawn)
	{
		float Distance = FVector::Dist(InPawn->GetActorLocation(), LocalPawn->GetActorLocation());
		if (Distance > 600)
		{
			SetDisplayText("");
		}
		else
		{
			FHitResult Hit;
			FCollisionQueryParams TraceParams;
			TraceParams.AddIgnoredActor(InPawn);
			TraceParams.AddIgnoredActor(LocalPawn);
			bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, InPawn->GetActorLocation(), LocalPawn->GetActorLocation(), ECC_Visibility, TraceParams);
			if (bHit && Hit.GetActor() != LocalPawn)
			{
				// Le joueur est derrière un mur
				SetDisplayText("");
			}
		}
	}	
}

void UOverheadWidget::NativeDestruct()
{
	RemoveFromParent();
	Super::NativeDestruct();
	
}
