// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactor.h"
#include "Net_Interactor.generated.h"


UENUM()
enum class EInteractorMode : uint8
{
	Single UMETA(DisplayName = "Single"),
	Multi UMETA(DisplayName = "Milti")
};

UCLASS(Abstract)
class INTERACTCORE_API UNet_Interactor : public UInteractor
{
	GENERATED_BODY()

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interactor|Setting")
	EInteractorMode HoverMode;
	UPROPERTY(EditAnywhere, Category = "Interactor|Selection", meta = (EditCondition = "HoverMode == EInteractorMode::Single", EditConditionHides))
	bool HigherPriority;
	// UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent, Category = "Interactor|Inside")
	// bool CanUpdateInteraction() const;
	// UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interactor|Inside")
	// TArray<FHitResult> DetectInteractables(TArray<TScriptInterface<IInteractable>> &Interactables);
	// UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interactor|Inside")
	// bool FilterHoverInteractables(TArray<TScriptInterface<IInteractable>> &Interactables);
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction|TEST")
	void InputBP(int id);
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|TEST")
	// TScriptInterface<IInteractable> InteractableRef;
};
