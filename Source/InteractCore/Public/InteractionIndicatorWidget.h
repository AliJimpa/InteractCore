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
	UInteractableComponent *OwnerInteractable = nullptr;

public:
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction|Override")
	void InitializeIndicator(UInteractableComponent *InOwner);
	virtual void InitializeIndicator_Implementation(UInteractableComponent *InOwner) { OwnerInteractable = InOwner;}
	UFUNCTION(BlueprintPure, Category = "Interaction|Getter", meta = (DisplayName = "Get Owner Interactable"))
	UInteractableComponent *GetOwnerInteractable() const { return OwnerInteractable; }

	UFUNCTION(BlueprintNativeEvent, Category = "Interaction|Override")
	void OnInteractionStateChanged(EInteractionState NewState);
	virtual void OnInteractionStateChanged_Implementation(EInteractionState NewState) {}
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction|Override")
	void OnInteractionProgress(float Progress);
	virtual void OnInteractionProgress_Implementation(float Progress) {}
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction|Override")
	void OnInteractionCompleted();
	virtual void OnInteractionCompleted_Implementation() {}
};
