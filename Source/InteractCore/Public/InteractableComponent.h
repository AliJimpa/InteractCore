// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "Interactable.h"
#include "InteractDebug.h"
#include "InteractableComponent.generated.h"

UENUM()
enum class EInteractionInputMode : uint8
{
	Press UMETA(DisplayName = "Press"),
	Release UMETA(DisplayName = "Release"),
	Tap UMETA(DisplayName = "Tap"),
	Hold UMETA(DisplayName = "Hold"),
	ChargedRelease UMETA(DisplayName = "ChargedRelease"),
	DoubleClick UMETA(DisplayName = "Double Click"),
	Any UMETA(DisplayName = "Any")
};
UENUM()
enum class EInteractionUsageMode : uint8
{
	Once UMETA(DisplayName = "Once"),
	Multi UMETA(DisplayName = "Multi"),
	Cooldown UMETA(DisplayName = "Cooldown")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionEvent, UInteractionComponent *, Provider);

UCLASS(Abstract, Blueprintable, ClassGroup = (InteractCore), meta = (Tooltip = "Base class for all Interactable component."))
class INTERACTCORE_API UInteractableComponent : public USphereComponent, public IInteractable
{
	GENERATED_BODY()
public:
	UInteractableComponent();

protected:
	virtual void OnRegister() override;

protected:
	// Implement IInteraction Interface
	virtual void Interact_Implementation(UInteractionComponent *Provider) override;
	virtual void Hover_Implementation(UInteractionComponent *Provider, FHitResult Hit) override;
	virtual void UnHover_Implementation(UInteractionComponent *Provider) override;
	virtual bool ShouldHandleInput_Implementation(const FInputActionInstance &InputValue) const override;

private:
	bool CanInteract() const;

private:
	bool bIsHovered = false;
	bool bHasInteracted = false;
	float NextAllowedInteractTime = 0.0f;
	mutable float LastPressTime = -1000.f;

public:
	UPROPERTY(BlueprintAssignable, Category = "Hover Events")
	FOnInteractionEvent OnHoverBegin;
	UPROPERTY(BlueprintAssignable, Category = "UnHover Events")
	FOnInteractionEvent OnHoverEnd;
	UPROPERTY(BlueprintAssignable, Category = "Interact Events")
	FOnInteractionEvent OnInteract;

private:
	UPROPERTY(EditAnywhere, Category = "Interaction", meta = (AllowPrivateAccess = "true"))
	bool EnableInteraction = true;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	TEnumAsByte<ECollisionChannel> InteractChannel = ECC_Camera;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	EInteractionUsageMode InteractMode = EInteractionUsageMode::Once;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction", meta = (EditCondition = "InteractMode == EInteractionUsageMode::Cooldown", EditConditionHides, ClampMin = "0.0"))
	float CooldowDuration = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Input")
	EInteractionInputMode InputMode = EInteractionInputMode::Press;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Input", meta = (EditCondition = "InputMode == EInteractionInputMode::Hold || InputMode == EInteractionInputMode::ChargedRelease", EditConditionHides))
	float HoldTimeThreshold = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Input", meta = (EditCondition = "InputMode == EInteractionInputMode::DoubleClick ", EditConditionHides))
	float DoubleTapInterval = 0.3f;

public:
	UFUNCTION(BlueprintPure, Category = "Interaction|Status")
	bool IsHovered() const { return bIsHovered; }
	UFUNCTION(BlueprintPure, Category = "Interaction|Status")
	bool IsEnableInteraction() const { return EnableInteraction; }
	UFUNCTION(BlueprintPure, Category = "Interaction|Status")
	bool HasInteracted() const { return bHasInteracted; }
	UFUNCTION(BlueprintCallable, Category = "Interaction|Functions")
	void SetEnableInteraction(bool Enable)
	{
		EnableInteraction = Enable;
	}
	UFUNCTION(BlueprintCallable, Category = "Interaction|Functions")
	void ResetInteraction()
	{
		bHasInteracted = false;
		NextAllowedInteractTime = 0.0f;
	}

protected:
	UFUNCTION(BlueprintPure, BlueprintNativeEvent, Category = "Interaction|Override")
	bool IsAllowedInteraction(UInteractionComponent *Interactor) const;
	virtual bool IsAllowedInteraction_Implementation(UInteractionComponent *Interactor) const
	{
		return EnableInteraction && !bHasInteracted;
	}
};
