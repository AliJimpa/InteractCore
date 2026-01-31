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
	bool active = false;
	FHitResult HitData;
};

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
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

private:
	static constexpr int32 MaxHits = 5;
	TStaticArray<FInteractTickRecord, MaxHits> DetectionData;
	TStaticArray<FInteractTickRecord, MaxHits> HoverData;
	void CheckUnhoverStage() {};
	void CheckHoverStage(TStaticArray<FInteractTickRecord, MaxHits> &InOutHits)
	{
		for (FInteractTickRecord &Detection : InOutHits)
		{
			if (!Detection.active)
			{
				continue;
			}
			Hover(Detection);
			// Add to Hover Data
		}
	};
	void Hover(FInteractTickRecord record) {};
	void Unhover(FInteractTickRecord record) {};
	bool DoesAnyRecordDetect(TStaticArray<FInteractTickRecord, MaxHits> &InOutHits) const
	{
		int32 ActiveIndex = INDEX_NONE;
		for (int32 i = 0; i < InOutHits.Num(); ++i)
		{
			if (InOutHits[i].active)
			{
				ActiveIndex = i;
				break;
			}
		}
		return ActiveIndex != INDEX_NONE;
	}
	void CompaireRecords(TStaticArray<FInteractTickRecord, MaxHits> &DetectHit, TStaticArray<FInteractTickRecord, MaxHits> &HoverHit)
	{

		bool bMatchFound = false;
		for (const FInteractTickRecord &Hover : HoverHit)
		{
			if (!Hover.active)
			{
				continue;
			}

			AActor *HoverActor = Hover.HitData.GetActor();
			bMatchFound = false;

			for (FInteractTickRecord &Detection : DetectHit)
			{
				if (!Detection.active)
				{
					continue;
				}

				if (Detection.HitData.GetActor() == HoverActor)
				{
					Detection.active = false;
					bMatchFound = true;
					break;
				}
			}

			if (!bMatchFound)
			{
				Unhover(Hover);
			}
		}
	}

protected:
	virtual bool CanUpdateInteraction() const PURE_VIRTUAL(UInteractor::CanUpdateInteraction2, return false;);
	virtual bool TryDetectStage(TStaticArray<FInteractTickRecord, MaxHits> &InOutHits) PURE_VIRTUAL(UInteractor::TryDetectStage, return false;);
	virtual bool TrySortStage(TStaticArray<FInteractTickRecord, MaxHits> &InOutHits) PURE_VIRTUAL(UInteractor::TrySortStage, return false;);

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
