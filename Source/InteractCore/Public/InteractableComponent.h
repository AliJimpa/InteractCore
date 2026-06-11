// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "Interactable.h"
#include "InteractDebug.h"
#include "InteractableComponent.generated.h"

UENUM()
enum class EInputInteractionType : uint8
{
	Press UMETA(DisplayName = "Press"),
	Release UMETA(DisplayName = "Release"),
	Tap UMETA(DisplayName = "Tap"),
	Hold UMETA(DisplayName = "Hold"),
	DoubleClick UMETA(DisplayName = "Double Click"),
	Any UMETA(DisplayName = "Any")
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
	virtual void BeginPlay() override;

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
	bool bIsEnableInteraction = true;

public:
	UPROPERTY(BlueprintAssignable, Category = "Hover Events")
	FOnInteractionEvent OnHoverBegin;
	UPROPERTY(BlueprintAssignable, Category = "UnHover Events")
	FOnInteractionEvent OnHoverEnd;
	UPROPERTY(BlueprintAssignable, Category = "Interact Events")
	FOnInteractionEvent OnInteract;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	bool StartWithAllowed = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	TEnumAsByte<ECollisionChannel> InteractChannel = ECC_Camera;

public:
	UFUNCTION(BlueprintPure, Category = "Interaction|Status")
	bool IsHovered() const { return bIsHovered; }
	UFUNCTION(BlueprintPure, Category = "Interaction|Status")
	bool IsEnableInteraction() const { return bIsEnableInteraction; }
	UFUNCTION(BlueprintCallable, Category = "Interaction|Status")
	void SetEnableInteraction(bool Enable)
	{
		bIsEnableInteraction = Enable;
	}

protected:
	UFUNCTION(BlueprintPure, BlueprintNativeEvent, Category = "Interaction|Override")
	bool IsAllowedInteraction(UInteractionComponent *Interactor) const;
	virtual bool IsAllowedInteraction_Implementation(UInteractionComponent *Interactor) const
	{
		return bIsEnableInteraction;
	}
};
