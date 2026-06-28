// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interface/InteractionIndicator.h"
#include "InteractionIndicatorWidget.generated.h"

class UInteractableComponent;

/**
 *
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class INTERACTCORE_API UInteractionIndicatorWidget : public UUserWidget, public IInteractionIndicator
{
	GENERATED_BODY()
protected:
	// Implement IInteractionIndicator Interface
	virtual void InitializeIndicator_Implementation(UObject *InOwner) override { OwnerInteractable = InOwner; }
	virtual void OnInteractionStateChanged_Implementation(EInteractionState NewState) override {}
	virtual void OnInteractionProgress_Implementation(float Progress) override {}
	virtual void OnInteractionCompleted_Implementation() override {}

private:
	UPROPERTY()
	UObject *OwnerInteractable = nullptr;

public:
	UFUNCTION(BlueprintPure, Category = "Interaction|Getter", meta = (DisplayName = "Get Owner Interactable"))
	UObject *GetOwnerInteractable() const { return OwnerInteractable; }
};
