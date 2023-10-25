// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterHUD.h"

void ABlasterHUD::DrawHUD()
{
	Super::DrawHUD();

	FVector2D ViewportSize;
	if (GEngine)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		const FVector2D ViewportCenter(ViewportSize.X / 2, ViewportSize.Y / 2);

		if (HUDPackage.CrosshairsCenter)
		{
			DrawCrosshairs(HUDPackage.CrosshairsCenter, ViewportCenter);
		}
		if (HUDPackage.CrosshairsLeft)
		{
			DrawCrosshairs(HUDPackage.CrosshairsLeft, ViewportCenter);
		}
		if (HUDPackage.CrosshairsRight)
		{
			DrawCrosshairs(HUDPackage.CrosshairsRight, ViewportCenter);
		}
		if (HUDPackage.CrosshairsTop)
		{
			DrawCrosshairs(HUDPackage.CrosshairsTop, ViewportCenter);
		}
		if (HUDPackage.CrosshairsBottom)
		{
			DrawCrosshairs(HUDPackage.CrosshairsBottom, ViewportCenter);
		}
	}


}

void ABlasterHUD::DrawCrosshairs(UTexture2D* Texture, FVector2D ViewportCenter)
{
	const float TextureWidth = Texture->GetSizeX();
	const float TextureHeight = Texture->GetSizeY();
	const FVector2D TextureDrawPoint(ViewportCenter.X - (TextureWidth / 2), ViewportCenter.Y - (TextureHeight / 2));

	DrawTexture(Texture, TextureDrawPoint.X, TextureDrawPoint.Y, TextureWidth, TextureHeight, 0, 0, 1, 1, FLinearColor::White);
}
