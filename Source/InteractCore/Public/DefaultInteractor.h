// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractionComponent.h"
#include "DefaultInteractor.generated.h"

UENUM()
enum class EInteractionAdaptiveTickMode : uint8
{
	CameraRotation UMETA(
		DisplayName = "Camera Rotation",
		ToolTip = "Speeds up tick when the camera rotation changes beyond the threshold."),

	OwnerVelocity UMETA(
		DisplayName = "Owner Movement Speed",
		ToolTip = "Speeds up tick when the owning actor's movement speed exceeds the threshold."),

	MouseMovement UMETA(
		DisplayName = "Mouse Movement",
		ToolTip = "Speeds up tick when the mouse cursor moves on screen beyond the threshold (pixels)."),

	Custom UMETA(
		DisplayName = "Custom",
		ToolTip = "Use Custom adaptive tick event. Component uses the 'CustomTickInterval' method."),
};
/**
 *
 */
UCLASS(Abstract)
class INTERACTCORE_API UDefaultInteractor : public UInteractionComponent
{
	GENERATED_BODY()

public:
	UDefaultInteractor();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

protected:
	virtual void CustomAdaptiveTick(float Threshould, float &OutTickRate) PURE_VIRTUAL(UDefaultInteractor::CustomAdaptiveTick);

private:
	void UpdateTickByCameraRotation(float Threshold, float &OutTickRate);
	TObjectPtr<APlayerCameraManager> CameraManager;
	FRotator LastCameraRotation;
	void UpdateTickByOwnerMovement(float Threshold, float &OutTickRate);
	void UpdateTickByMouse(int32 Threshold, float &OutTickRate);
	FVector2D LastMousePosition;
	bool bIsImplementCustomAdaptiveTick = false;

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction|Override")
	void K2_CustomAdaptiveTick(float Threshould, UPARAM(ref) float &OutTickRate);

private:
	UPROPERTY(EditAnywhere, Category = "Interaction|Tick", meta = (ToolTip = "Automatically adjust trace frequency based on camera movement. Faster when the player moves the camera and slower when idle."))
	bool bDynamicInterval = false;
	UPROPERTY(EditAnywhere, Category = "Interaction|Tick", meta = (EditCondition = "bDynamicInterval", EditConditionHides, ToolTip = "Select what input/activity drives the adaptive tick rate."))
	EInteractionAdaptiveTickMode AdaptiveTickMode = EInteractionAdaptiveTickMode::CameraRotation;
	UPROPERTY(EditAnywhere, Category = "Interaction|Tick", meta = (EditCondition = "bDynamicInterval", EditConditionHides, ClampMin = "0.008", ClampMax = "1"))
	float FastTickRate = 0.016f; // ~60fps
	UPROPERTY(EditAnywhere, Category = "Interaction|Tick", meta = (EditCondition = "bDynamicInterval", EditConditionHides, ClampMin = "0.008", ClampMax = "1"))
	float SlowTickRate = 0.1f; // 10fps

	UPROPERTY(EditAnywhere, Category = "Interaction|Tick", meta = (EditCondition = "bDynamicInterval && AdaptiveTickMode == EInteractionAdaptiveTickMode::CameraRotation", EditConditionHides, ClampMin = "0.1", ClampMax = "45.0"))
	float ThresholdCameraRotation = 1.5f;
	UPROPERTY(EditAnywhere, Category = "Interaction|Tick", meta = (EditCondition = "bDynamicInterval && AdaptiveTickMode == EInteractionAdaptiveTickMode::OwnerVelocity", EditConditionHides, ClampMin = "0.0", ClampMax = "1000.0"))
	float ThresholdOwnerVelocity = 10;
	UPROPERTY(EditAnywhere, Category = "Interaction|Tick", meta = (EditCondition = "bDynamicInterval && AdaptiveTickMode == EInteractionAdaptiveTickMode::MouseMovement", EditConditionHides, ClampMin = "0", ClampMax = "100"))
	int32 ThresholdMouseMovement = 3;
	UPROPERTY(EditAnywhere, Category = "Interaction|Tick", meta = (EditCondition = "bDynamicInterval && AdaptiveTickMode == EInteractionAdaptiveTickMode::Custom", EditConditionHides, ClampMin = "0.001"))
	float ThresholdCustom = 0.03f;
};
