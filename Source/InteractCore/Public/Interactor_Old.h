// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interactor_Old.generated.h"

struct FInputActionInstance;
class UInputAction;
class IInteractable;
enum class ETriggerEvent : uint8;

// The total value can allocate in stack for usein this component
#define MAXHITS 5

struct FInteractionData2
{
	bool active = false;
	FHitResult HitData;
};

struct FInteractionBuffer
{
private:
	// TScriptInterface<IInteractable> Interface;
	// bool bIsHovered = false;
	TStaticArray<FInteractionData2, MAXHITS> Data;
	int32 AvailableCount = MAXHITS;

public:
	/** Number of valid elements currently in the buffer */
	FORCEINLINE int32 Num() const
	{
		return AvailableCount;
	}

	/** Maximum capacity */
	FORCEINLINE int32 Max() const
	{
		return MAXHITS;
	}

	FORCEINLINE bool IsEmpty() const
	{
		return AvailableCount == 0;
	}

	FORCEINLINE bool IsFull() const
	{
		return AvailableCount >= MAXHITS;
	}

	/** Read-only access */
	FORCEINLINE const FInteractionData2 &operator[](int32 Index) const
	{
		check(Index >= 0 && Index < AvailableCount);
		return Data[Index];
	}

	/** Mutable access */
	FORCEINLINE FInteractionData2 &operator[](int32 Index)
	{
		check(Index >= 0 && Index < AvailableCount);
		return Data[Index];
	}

	/** Add element, returns false if buffer is full */
	// FORCEINLINE bool Add(const FInteractionData2 &Item)
	// {
	// 	if (IsFull())
	// 	{
	// 		return false;
	// 	}

	// 	//Data[NumElements++] = Item;
	// 	return true;
	// }

	/** Emplace-like add */
	// FORCEINLINE FInteractionData2 &AddDefaulted()
	// {
	// 	check(!IsFull());
	// 	return Data[NumElements++];
	// }

	/** Remove element by index (swap-remove, O(1)) */
	// FORCEINLINE void RemoveAtSwap(int32 Index)
	// {
	// 	check(Index >= 0 && Index < NumElements);

	// 	--NumElements;
	// 	if (Index != NumElements)
	// 	{
	// 		Data[Index] = MoveTemp(Data[NumElements]);
	// 	}
	// }

	/** Clear buffer */
	// FORCEINLINE void Reset()
	// {
	// 	NumElements = 0;
	// }

	/** Pointer access (for ranged-for compatibility) */
	FORCEINLINE FInteractionData2 *GetData()
	{
		return Data.GetData();
	}

	FORCEINLINE const FInteractionData2 *GetData() const
	{
		return Data.GetData();
	}

	/** Range-for support */
	FORCEINLINE FInteractionData2 *begin() { return Data.GetData(); }
	FORCEINLINE FInteractionData2 *end() { return Data.GetData() + AvailableCount; }

	FORCEINLINE const FInteractionData2 *begin() const { return Data.GetData(); }
	FORCEINLINE const FInteractionData2 *end() const { return Data.GetData() + AvailableCount; }
};

UCLASS(Abstract)
class INTERACTCORE_API UInteractor_Old : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInteractor_Old();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	int32 GetNumActive(const FInteractionBuffer &Buffer) const
	{
		int32 Count = 0;

		for (int32 i = 0; i < Buffer.Num(); ++i)
		{
			if (Buffer[i].active)
			{
				++Count;
			}
		}

		return Count;
	}
	int32 FindFirstActiveIndex(const FInteractionBuffer &Buffer) const
	{
		for (int32 i = 0; i < Buffer.Num(); ++i)
		{
			if (Buffer[i].active)
			{
				return i;
			}
		}

		return INDEX_NONE;
	}
	bool HasAnyActive(const FInteractionBuffer &Buffer) const
	{
		for (int32 i = 0; i < Buffer.Num(); ++i)
		{
			if (Buffer[i].active)
			{
				return true;
			}
		}
		return false;
	}

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

private:
	// Variables
	FInteractionBuffer DetectionData;
	FInteractionBuffer HoverData;
	// Functions
	// Get owner control with trace ownership
	AController *ResolveControllerFromOwnership() const;
	// Binding methods for interactionInput
	void OnInteractInput_Triggered(const FInputActionInstance &Instance);
	void OnInteractInput_Started(const FInputActionInstance &Instance);
	void OnInteractInput_Ongoing(const FInputActionInstance &Instance);
	void OnInteractInput_Canceled(const FInputActionInstance &Instance);
	void OnInteractInput_Completed(const FInputActionInstance &Instance);
	//
	void OnInteractTrigger(const FInputActionInstance &Instance, ETriggerEvent TriggerMode);
	//
	bool CanUpdateInteraction(bool UseInput) const;
	void CheckUnhoverStage(FInteractionBuffer &Buffer);
	void CheckHoverStage(FInteractionBuffer &Buffer);
	//
	bool DetectionStage(FInteractionBuffer &Buffer);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interactor|Input")
	UInputAction *InteractionInput;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interactor|Input")
	bool UseInputForHovering;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interactor|Input", meta = (EditCondition = "UseInputForHovering == true", EditConditionHides))
	UInputAction *HoverInput;
	UFUNCTION(BlueprintPure, Category = "Interactor|Input")
	bool IsHoverInputPressed() const;

	// virtual void OnInteractTrigger(const FInputActionInstance &Instance, ETriggerEvent TriggerMode) PURE_VIRTUAL(UInteractor_Old::OnInteractTrigger, );
	//  UFUNCTION()
	// virtual bool CanUpdateInteraction() const PURE_VIRTUAL(UInteractor_Old::CanUpdateInteraction, return false;);
	// UFUNCTION()
	// virtual bool TryDetectStage(FInteractionBuffer &InOutHits) PURE_VIRTUAL(UInteractor_Old::TryDetectStage, return false;);
	// UFUNCTION()
	void Hover(FInteractionData2 record) {};
	// UFUNCTION()
	void Unhover(FInteractionData2 record)
	{
		record.active = false;
	};
	// UFUNCTION()
	void CompaireRecords(FInteractionBuffer &DetectHit, FInteractionBuffer &HoverHit)
	{

		bool bMatchFound = false;
		for (const FInteractionData2 &Hover : HoverHit)
		{
			if (!Hover.active)
			{
				continue;
			}

			AActor *HoverActor = Hover.HitData.GetActor();
			bMatchFound = false;

			for (FInteractionData2 &Detection : DetectHit)
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
	virtual bool TrySortStage(FInteractionBuffer &InOutHits) PURE_VIRTUAL(UInteractor_Old::TrySortStage, return false;);
	// UFUNCTION()
};
