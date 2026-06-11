// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractionIndicatorWidget.generated.h"

class UInteractableComponent;

UENUM(BlueprintType)
enum class EInteractionState : uint8
{
	Detected UMETA(DisplayName = "Detected"),
	Undetected UMETA(DisplayName = "Undetected"),
	Focused UMETA(DisplayName = "Focused"),
	Unfocused UMETA(DisplayName = "Unfocused")
};
/**
 *
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class INTERACTCORE_API UInteractionIndicatorWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction|Override")
	void OnInteractionStateChanged(EInteractionState NewState);
	virtual void OnInteractionStateChanged_Implementation(EInteractionState NewState) {}
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction|Override")
	void OnInteractionProgress(UInteractableComponent* comp, float Progress);
	virtual void OnInteractionProgress_Implementation(UInteractableComponent* comp, float Progress) {}
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction|Override")
	void OnInteractionCompleted();
	virtual void OnInteractionCompleted_Implementation() {}
};
