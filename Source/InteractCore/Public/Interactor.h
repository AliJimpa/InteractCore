// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Containers/StaticArray.h"
#include "Interactor.generated.h"

struct FInputActionInstance;
class UInputAction;
class IInteractable;

struct FInteractTickRecord
{
	// UObject* ObjectClass;
	// TScriptInterface<IInteractable> Interface;
	FHitResult HitData;
};

UCLASS(Abstract)
class INTERACTCORE_API UInteractor : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInteractor();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	AController *ResolveControllerFromOwnership() const;
	void OnTriggered(const FInputActionInstance &Instance);
	void OnStarted(const FInputActionInstance &Instance);
	void OnOngoing(const FInputActionInstance &Instance);
	void OnCanceled(const FInputActionInstance &Instance);
	void OnCompleted(const FInputActionInstance &Instance);

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

private:
	static constexpr int32 MaxHits = 5;
	TStaticArray<FInteractTickRecord, MaxHits> DetectionData;

protected:
	virtual bool CanUpdateInteraction() const PURE_VIRTUAL(UInteractor::CanUpdateInteraction2 , return false;);
	virtual bool TryDetectInteractables(TStaticArray<FInteractTickRecord, MaxHits> &InOutHits) PURE_VIRTUAL(UInteractor::TryDetectInteractables, return false;);

	// UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	// TArray<TScriptInterface<IInteractable>> DetectedInteractables;
	// UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	// TArray<TScriptInterface<IInteractable>> HoverdInteractables;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interactor|Input")
	UInputAction *InputAction;
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
