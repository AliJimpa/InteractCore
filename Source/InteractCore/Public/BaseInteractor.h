// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
// #include "Containers/StaticArray.h"
#include "BaseInteractor.generated.h"

struct FInputActionInstance;
class UInputAction;
class IInteractable;
enum class ETriggerEvent : uint8;

struct FInteractTickRecord
{
	// UObject* ObjectClass;
	// TScriptInterface<IInteractable> Interface;
	bool active = false;
	FHitResult HitData;
};
template <int32 MaxHits>
struct THit 
{
	TStaticArray<FInteractTickRecord, MaxHits> DetectionData;

	FORCEINLINE RecordType &operator[](int32 Index)
	{
		return DetectionData[Index];
	}

	FORCEINLINE const RecordType &operator[](int32 Index) const
	{
		return DetectionData[Index];
	}
};

UCLASS(Abstract)
class INTERACTCORE_API UBaseInteractor : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UBaseInteractor();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

private:
	// Variables
	static constexpr int32 MaxHits = 5;
	TStaticArray<FInteractTickRecord, MaxHits> DetectionData;
	TStaticArray<FInteractTickRecord, MaxHits> HoverData;
	// Functions
	AController *ResolveControllerFromOwnership() const;
	void OnInteractInput_Triggered(const FInputActionInstance &Instance);
	void OnInteractInput_Started(const FInputActionInstance &Instance);
	void OnInteractInput_Ongoing(const FInputActionInstance &Instance);
	void OnInteractInput_Canceled(const FInputActionInstance &Instance);
	void OnInteractInput_Completed(const FInputActionInstance &Instance);
	void CheckUnhoverStage() {};

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interactor|Input")
	UInputAction *InteractionInput;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interactor|Input")
	bool UseInputForHovering;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interactor|Input", meta = (EditCondition = "UseInputForHovering == true", EditConditionHides))
	UInputAction *HoverInput;

	virtual void OnInteractTrigger(ETriggerEvent TriggerMode) PURE_VIRTUAL(UBaseInteractor::OnInteractTrigger, );
	// UFUNCTION()
	virtual bool CanUpdateInteraction() const PURE_VIRTUAL(UBaseInteractor::CanUpdateInteraction, return false;);
	// UFUNCTION()
	virtual bool TryDetectStage(TStaticArray<FInteractTickRecord, MaxHits> &InOutHits) PURE_VIRTUAL(UBaseInteractor::TryDetectStage, return false;);
	// UFUNCTION()
	void Hover(FInteractTickRecord record) {};
	// UFUNCTION()
	void Unhover(FInteractTickRecord record) {};
	// UFUNCTION()
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
	};
	// UFUNCTION()
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
	};
	// UFUNCTION()
	virtual bool TrySortStage(TStaticArray<FInteractTickRecord, MaxHits> &InOutHits) PURE_VIRTUAL(UBaseInteractor::TrySortStage, return false;);
	// UFUNCTION()
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
};
