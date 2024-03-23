#pragma once
UENUM(BlueprintType)
enum class EUpscaleMode : uint8
{
	EUM_Default UMETA(DisplayName = "Default"),
	EUM_DLSS UMETA(DisplayName = "NVIDIA DLSS"),
	EUM_ImageScaling UMETA(DisplayName = "NVIDIA Image Scaling"),
	EUM_Max UMETA(DisplayName = "DefaultMax")
};