// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseInteractor.h"
#include "Interactor.generated.h"


UENUM()
enum class EInteractorMode : uint8
{
	Single UMETA(DisplayName = "Single"),
	Multi UMETA(DisplayName = "Milti")
};

UCLASS(Abstract)
class INTERACTCORE_API UInteractor : public UBaseInteractor
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInteractor();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;


	// UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	// TArray<TScriptInterface<IInteractable>> DetectedInteractables;
	// UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	// TArray<TScriptInterface<IInteractable>> HoverdInteractables;

protected:
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interactor|Input")
	// UInputAction *InputAction;
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
	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction|TEST")
	void InputBP(int id);
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|TEST")
	// TScriptInterface<IInteractable> InteractableRef;
};
