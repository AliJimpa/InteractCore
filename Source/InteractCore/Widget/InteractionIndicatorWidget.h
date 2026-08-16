// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractionIndicatorWidget.generated.h"

class UInteractableComponent;

UENUM(BlueprintType)
enum class EInteractionState : uint8
{
	Begindetection UMETA(DisplayName = "Begin Detection"),
	Enddetection UMETA(DisplayName = "End Detection"),
	Beginhover UMETA(DisplayName = "Begin Hover"),
	Endhover UMETA(DisplayName = "End Hover")
};

/**
 *
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class INTERACTCORE_API UInteractionIndicatorWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	UPROPERTY()
	UObject *OwnerInteractable = nullptr;

public:
	void InitializeIndicator(UObject *InOwner)
	{
		OwnerInteractable = InOwner;
	}

public:
	UFUNCTION(BlueprintPure, Category = "Interaction|Getter", meta = (DisplayName = "Get Owner Interactable"))
	UObject *GetOwnerInteractable() const { return OwnerInteractable; }

	UFUNCTION(BlueprintNativeEvent, Category = "InteractionIndicator|Events", meta = (ToolTip = "Called whenever the interaction state changes."))
	void OnInteractionStateChanged(EInteractionState NewState);
	virtual void OnInteractionStateChanged_Implementation(EInteractionState NewState) {}
	UFUNCTION(BlueprintNativeEvent, Category = "InteractionIndicator|Events", meta = (ToolTip = "Called to update the current interaction progress."))
	void OnInteractionProgress(float Progress);
	virtual void OnInteractionProgress_Implementation(float Progress) {}
	UFUNCTION(BlueprintNativeEvent, Category = "InteractionIndicator|Input", meta = (ToolTip = "Called when the interaction has been successfully completed.."))
	void OnInteractionCompleted();
	virtual void OnInteractionCompleted_Implementation() {}
};
