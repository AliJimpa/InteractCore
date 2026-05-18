// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractDebug.h"
#include "InteractionComponent.generated.h"

class IInteractable;
class UInputAction;
struct FInputActionInstance;
class UEnhancedInputComponent;

UENUM()
enum class EInteractionSearchMode : uint8
{
	ActorOnly UMETA(
		DisplayName = "Actor Only",
		ToolTip = "Only checks the hit Actor for the interaction interface."),

	ComponentOnly UMETA(
		DisplayName = "Component Only",
		ToolTip = "Only checks the hit Components for the interaction interface."),

	ActorAndComponent UMETA(
		DisplayName = "Actor And Component",
		ToolTip = "Checks both the hit Actor and its Components for the interaction interface."),

	PreferActorFallbackToComponent UMETA(
		DisplayName = "Prefer Actor, Fallback To Component",
		ToolTip = "Checks the Actor first. If the Actor does not implement the interface, searches Components for a valid implementation.")
};

UENUM()
enum class EInteractionTickMode : uint8
{
	EveryFrame UMETA(
		DisplayName = "Every Frame",
		ToolTip = "Run the interaction trace every frame. Highest responsiveness but most expensive."),

	FixedInterval UMETA(
		DisplayName = "Fixed Interval",
		ToolTip = "Run the interaction trace at a fixed time interval. Good balance between responsiveness and performance."),

	AdaptiveInterval UMETA(
		DisplayName = "Adaptive Interval",
		ToolTip = "Adjust trace frequency depending on camera movement."),
	CameraAdaptive UMETA(
		DisplayName = "Camera Adaptive",
		ToolTip = "Automatically adjust trace frequency based on camera movement. Faster when the player moves the camera and slower when idle.")
};

UCLASS(Abstract)
class INTERACTCORE_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInteractionComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override final;
	virtual void PreHovering() PURE_VIRTUAL(UInteractionComponent::PreHovering);
	virtual bool TryGetDetectedFocused(AController *Controller, FHitResult &OutHit) const PURE_VIRTUAL(UInteractionComponent::TryGetDetectedFocused, return false;);
	virtual void PostHovering() PURE_VIRTUAL(UInteractionComponent::PostHovering);
	virtual bool TryUpdateAdaptiveTick(float Threshould, float &OutTickRate);

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override final;

private:
	TScriptInterface<IInteractable> CurrentInteractable = nullptr;
	void UpdateInteraction();
	TScriptInterface<IInteractable> ResolveInteractableFromHit(const FHitResult &Hit) const;

	TObjectPtr<AController> MyController;
	AController *ResolveControllerFromOwnership() const;

	bool bIsImp_AdaptiveTick = false;
	TObjectPtr<APlayerCameraManager> CameraManager;
	FRotator LastCameraRotation;

	void SetupInteractionInput(AController *Controller);
	void BindInteractionInput(UEnhancedInputComponent *EIC);
	void OnInteractInput(const FInputActionInstance &Instance);

public:
	UFUNCTION(BlueprintPure, Category = "Interaction|Getter")
	EInteractionSearchMode GetMode() const { return DetectionMode; }
	UFUNCTION(BlueprintPure, Category = "Interaction|Getter")
	const TScriptInterface<IInteractable> GetCurrentInteractable() const { return CurrentInteractable; }
	UFUNCTION(BlueprintPure, Category = "Interaction|Getter")
	virtual FTransform GetPivot() const PURE_VIRTUAL(UInteractionComponent::GetPivot, return FTransform::Identity;);

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction|Override")
	bool K2_TryUpdateAdaptiveTick(float Threshould, UPARAM(ref) float &OutTickRate);

private:
	UPROPERTY(EditAnywhere, Category = "Interaction")
	EInteractionSearchMode DetectionMode = EInteractionSearchMode::ActorAndComponent;
	UPROPERTY(EditAnywhere, Category = "Interaction|Tick", meta = (ToolTip = "Automatically adjust trace frequency based on camera movement. Faster when the player moves the camera and slower when idle."))
	bool bAdaptiveInterval = false;
	UPROPERTY(EditAnywhere, Category = "Interaction|Tick", meta = (EditCondition = "bAdaptiveInterval", EditConditionHides, ClampMin = "0.001"))
	float AdaptiveIntervalThreshold = 0.03f;
	UPROPERTY(EditAnywhere, Category = "Interaction|Input")
	bool bInteractionInputBound = true;
	UPROPERTY(EditAnywhere, Category = "Interaction|Input", meta = (EditCondition = "bInteractionInputBound == true", EditConditionHides))
	UInputAction *InteractionInput;
};
