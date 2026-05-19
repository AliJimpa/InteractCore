// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractionComponent.h"
#include "DefaultInteraction.generated.h"

UENUM()
enum class EInteractionAdaptiveTickTrigger : uint8
{
	PivotRotation UMETA(
		DisplayName = "Pivot Rotation",
		ToolTip = "Speeds up ticking when the tracked pivot rotation changes beyond the threshold."),

	PivotPosition UMETA(
		DisplayName = "Pivot Position",
		ToolTip = "Speeds up ticking when the tracked pivot position changes beyond the threshold."),

	PivotTransform UMETA(
		DisplayName = "Pivot Transform",
		ToolTip = "Speeds up ticking when either pivot rotation or position changes beyond the threshold."),

	OwnerVelocity UMETA(
		DisplayName = "Owner Velocity",
		ToolTip = "Speeds up ticking when the owning actor's movement speed exceeds the threshold."),

	MouseMovement UMETA(
		DisplayName = "Mouse Movement",
		ToolTip = "Speeds up ticking when the mouse cursor moves on screen beyond the threshold (pixels)."),

	Custom UMETA(
		DisplayName = "Custom",
		ToolTip = "Use a custom adaptive tick rule implemented by the component."),
};

/**
 * Default interaction implementation layer.
 *
 * Extends UInteractionComponent and provides shared logic used by
 * common interaction types such as FPS or TPS interaction systems.
 * This class is not intended to be used directly.
 */
UCLASS(Abstract, Blueprintable, ClassGroup = (InteractCore), meta = (Tooltip = "Core framework for standard interactions."))
class INTERACTCORE_API UDefaultInteraction : public UInteractionComponent
{
	GENERATED_BODY()

public:
	UDefaultInteraction();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

protected:
	virtual bool CustomAdaptiveTick(float Threshould) PURE_VIRTUAL(UDefaultInteraction::CustomAdaptiveTick, return false;);
	virtual bool CanHover(UObject *Interactable) const override;
	virtual bool CanInteract(UObject *Interactable) const override;

private:
	bool bIsImplementCustomAdaptiveTick = false;
	bool bIsImplememtCanHover = false;
	bool bIsImplememtCanInteract = false;

	void UpdateAdaptiveTickRate();
	FTransform PreviousPivotTransform;
	FVector2D PreviousMousePosition;
	float CurrentInterval;

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction|Override", meta = (DisplayName = "CustomAdaptiveTick"))
	bool K2_CustomAdaptiveTick(float Threshould);
	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction|Override", meta = (DisplayName = "CanHover"))
	bool K2_CanHover(UObject *Interactable) const;
	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction|Override", meta = (DisplayName = "CanInteract"))
	bool K2_CanInteract(UObject *Interactable) const;

private:
	UPROPERTY(EditAnywhere, Category = "Interaction|Tick", meta = (ToolTip = "Automatically adjust trace frequency based on camera movement. Faster when the player moves the camera and slower when idle."))
	bool bDynamicInterval = false;
	UPROPERTY(EditAnywhere, Category = "Interaction|Tick", meta = (EditCondition = "bDynamicInterval", EditConditionHides, ToolTip = "Select what input/activity drives the adaptive tick rate."))
	EInteractionAdaptiveTickTrigger AdaptiveTickMode = EInteractionAdaptiveTickTrigger::PivotRotation;
	UPROPERTY(EditAnywhere, Category = "Interaction|Tick", meta = (EditCondition = "bDynamicInterval", EditConditionHides, ClampMin = "0.008", ClampMax = "1"))
	float FastTickRate = 0.033f; // ~30fps
	UPROPERTY(EditAnywhere, Category = "Interaction|Tick", meta = (EditCondition = "bDynamicInterval", EditConditionHides, ClampMin = "0.008", ClampMax = "1"))
	float SlowTickRate = 0.1f; // 10fps

	// Thresholds for the triggers
	UPROPERTY(EditAnywhere, Category = "Interaction|Tick", meta = (EditCondition = "bDynamicInterval && AdaptiveTickMode == EInteractionAdaptiveTickTrigger::PivotRotation || AdaptiveTickMode == EInteractionAdaptiveTickTrigger::PivotTransform", EditConditionHides, ClampMin = "0.0", ToolTip = "Rotation delta (degrees) required for fast ticking."))
	float RotationThreshold = 1.0f;
	UPROPERTY(EditAnywhere, Category = "Interaction|Tick", meta = (EditCondition = "bDynamicInterval && AdaptiveTickMode == EInteractionAdaptiveTickTrigger::PivotPosition || AdaptiveTickMode == EInteractionAdaptiveTickTrigger::PivotTransform", EditConditionHides, ClampMin = "0.0", ToolTip = "Position delta (cm) required for fast ticking."))
	float PositionThreshold = 2.0f;
	UPROPERTY(EditAnywhere, Category = "Interaction|Tick", meta = (EditCondition = "bDynamicInterval && AdaptiveTickMode == EInteractionAdaptiveTickTrigger::OwnerVelocity", EditConditionHides, ClampMin = "0.0", ToolTip = "Owner velocity (cm/s) required for fast ticking."))
	float VelocityThreshold = 10.0f;
	UPROPERTY(EditAnywhere, Category = "Interaction|Tick", meta = (EditCondition = "bDynamicInterval && AdaptiveTickMode == EInteractionAdaptiveTickTrigger::MouseMovement", EditConditionHides, ClampMin = "0.0", ToolTip = "Mouse movement (pixels) required for fast ticking."))
	float MouseMovementThreshold = 5.0f;
	UPROPERTY(EditAnywhere, Category = "Interaction|Tick", meta = (EditCondition = "bDynamicInterval && AdaptiveTickMode == EInteractionAdaptiveTickTrigger::Custom", EditConditionHides, ClampMin = "0.001"))
	float ThresholdCustom = 0.03f;
};
