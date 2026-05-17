// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractDebug.h"
#include "Interactable.h"
#include "Interactor.generated.h"

class UInputAction;
enum class EInteractionSearchMode : uint8;

// The total value can allocate in stack for usein this component
#define MAXHITS 5

// UENUM()
// enum class EInteractionSearchMode : uint8
// {
// 	ActorOnly UMETA(
// 		DisplayName = "Actor Only",
// 		ToolTip = "Only checks the hit Actor for the interaction interface."),

// 	ComponentOnly UMETA(
// 		DisplayName = "Component Only",
// 		ToolTip = "Only checks the hit Components for the interaction interface."),

// 	ActorAndComponent UMETA(
// 		DisplayName = "Actor And Component",
// 		ToolTip = "Checks both the hit Actor and its Components for the interaction interface."),

// 	PreferActorFallbackToComponent UMETA(
// 		DisplayName = "Prefer Actor, Fallback To Component",
// 		ToolTip = "Checks the Actor first. If the Actor does not implement the interface, searches Components for a valid implementation.")
// };

USTRUCT()
struct FInteractionRecord
{
	GENERATED_BODY()
private:
	UPROPERTY()
	bool bValid = false;
	UPROPERTY()
	FHitResult Hit;
	UPROPERTY()
	mutable TScriptInterface<IInteractable> Interface;

	FORCEINLINE void SetInterface(UObject *Obj) const
	{
		if (!Obj)
			return;

		if (Obj->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
		{
			Interface.SetObject(Obj);
			Interface.SetInterface(Cast<IInteractable>(Obj));
		}
	}

public:
	FORCEINLINE bool operator==(const FInteractionRecord &Other) const
	{
		return bValid == Other.bValid &&
			   Interface.GetObject() == Other.Interface.GetObject();
	}
	FORCEINLINE bool operator!=(const FInteractionRecord &Other) const
	{
		return !(*this == Other);
	}
	FORCEINLINE void SetHit(const FHitResult &InHit)
	{
		Interface = nullptr;
		bValid = Hit.bBlockingHit;
		if (bValid)
			Hit = InHit;
	}
	FORCEINLINE FHitResult GetHit() const
	{
		return Hit;
	}
	FORCEINLINE bool TryGetInterface(EInteractionSearchMode Mode, UObject *&OutObject) const
	{
		const TScriptInterface<IInteractable> Result = GetInterface(Mode);

		if (UObject *Obj = Result.GetObject())
		{
			OutObject = Obj;
			return true;
		}

		OutObject = nullptr;
		return false;
	}
	FORCEINLINE TScriptInterface<IInteractable> GetInterface(EInteractionSearchMode Mode) const
	{
		if (!bValid)
			return nullptr;

		if (Interface.GetObject())
			return Interface;

		AActor *HitActor = Hit.GetActor();
		UActorComponent *HitComp = Hit.GetComponent();
		if (!HitActor && !HitComp)
			return nullptr;

		switch (Mode)
		{
		case EInteractionSearchMode::ActorOnly:
		{
			if (HitActor && HitActor->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
			{
				SetInterface(HitActor);
			}
			break;
		}
		case EInteractionSearchMode::ComponentOnly:
		{
			if (HitComp && HitActor->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
			{
				SetInterface(HitComp);
			}
			break;
		}
		case EInteractionSearchMode::ActorAndComponent:
		{
			if (HitActor && HitActor->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
			{
				SetInterface(HitActor);
			}
			else if (HitComp && HitActor->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
			{
				SetInterface(HitComp);
			}
			break;
		}
		case EInteractionSearchMode::PreferActorFallbackToComponent:
		{
			if (HitActor && HitActor->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
			{
				SetInterface(HitActor);
				break;
			}

			if (HitActor)
			{
				TInlineComponentArray<UActorComponent *> Components(HitActor);
				for (UActorComponent *Comp : Components)
				{
					if (Comp->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
					{
						SetInterface(Comp);
						break;
					}
				}
			}
			break;
		}
		}
		return Interface;
	}
	FORCEINLINE bool HasInterface(EInteractionSearchMode Mode) const
	{
		return GetInterface(Mode) != nullptr;
	}
	FORCEINLINE bool IsValid() const
	{
		return bValid;
	}
	FORCEINLINE void Clear()
	{
		if (bValid)
		{
			bValid = false;
			Interface = nullptr;
			Hit.Reset(1.f, false);
		}
	}
};

USTRUCT()
struct FInteractionData
{
	GENERATED_BODY()
private:
	TStaticArray<FInteractionRecord, MAXHITS> Data;

public:
	FORCEINLINE FInteractionData &operator=(const FInteractionData &Other)
	{
		if (this != &Other)
		{
			Data = Other.Data;
		}
		return *this;
	}
	FORCEINLINE const FInteractionRecord &operator[](int32 Index) const
	{
		return Data[Index]; // returns by const reference
	}
	TArray<TScriptInterface<IInteractable>> GetInteractables(EInteractionSearchMode Mode) const
	{
		TArray<TScriptInterface<IInteractable>> Interactables;

		for (int32 i = 0; i < MAXHITS; ++i)
		{
			if (!Data[i].IsValid())
				continue;

			UObject *Obj;
			if (Data[i].TryGetInterface(Mode, Obj))
			{
				TScriptInterface<IInteractable> Interface;
				Interface.SetObject(Obj);
				Interface.SetInterface(Cast<IInteractable>(Obj));

				Interactables.Add(Interface);
			}
		}

		return Interactables;
	}
	FORCEINLINE const FInteractionRecord *FindRecordByInterface(const UObject *InObject, EInteractionSearchMode Mode) const
	{
		if (!InObject)
			return nullptr;

		for (const FInteractionRecord &Record : Data)
		{
			if (!Record.IsValid())
				continue;

			const TScriptInterface<IInteractable> Interface = Record.GetInterface(Mode);
			if (Interface.GetObject() == InObject)
			{
				return &Record;
			}
		}

		return nullptr;
	}
	FORCEINLINE void SetHit(int32 index, const FHitResult &InHit)
	{
		Data[index].SetHit(InHit);
	}
	FORCEINLINE bool HasAnyRecord() const
	{
		for (int32 j = 0; j < MAXHITS; ++j)
		{
			if (Data[j].IsValid())
				return true;
		}
		return false;
	}
	FORCEINLINE void Clear()
	{
		for (int32 Index = 0; Index < MAXHITS; ++Index)
		{
			Data[Index].Clear();
		}
	}
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
	virtual bool PerformSingleTrace(FInteractionData &OutData) PURE_VIRTUAL(UInteractor::PerformSingleTrace, return false;);
	virtual bool PerformMultiTrace(FInteractionData &OutData) PURE_VIRTUAL(UInteractor::PerformMultiTrace, return false;);
	virtual const FInteractionRecord *SelectInteractionTarget(const FInteractionData &HoverData) PURE_VIRTUAL(UInteractor::SelectInteractionTarget, return {};);

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

private:
	bool bHasSingleTrace = false;
	bool bHasMultiTrace = false;
	bool bHasSelectInteraction = false;
	FInteractionData CurrentDetectionData;
	FInteractionData ActiveHoverData;
	const FInteractionRecord *TargetInteraction = nullptr;
	bool PerformTrace(FInteractionData &DetectionData);
	bool EvaluateTraceHits(const FInteractionData &NewData, FInteractionData &OldData);
	const FInteractionRecord *ChooseInteractionTarget(const FInteractionData &HoverData);

private:
	UPROPERTY(EditAnywhere, Category = "Interaction")
	bool MultiHovering = false;
	UPROPERTY(VisibleAnywhere, Category = "Interaction", meta = (EditCondition = "MultiHovering == true", EditConditionHides))
	int MaxHovering = MAXHITS;
	UPROPERTY(EditAnywhere, Category = "Interaction")
	EInteractionSearchMode DetectionMode = EInteractionSearchMode::ActorAndComponent;
	UPROPERTY(EditAnywhere, Category = "Interaction|Input")
	UInputAction *InteractionInput;

protected:
	// UFUNCTION(BlueprintCallable, Category = "Interaction")
	// static void AddHIT(UPARAM(ref) FInteractionData &Buffer, const FHitResult hit)
	// {
	// 	Buffer.SetHit(hit);
	// }
	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction|Override")
	FHitResult K2_PerformSingleTrace();
	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction|Override")
	TArray<FHitResult> K2_PerformMultiTrace();
	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction|Override")
	TScriptInterface<IInteractable> K2_SelectInteractionTarget(const TArray<TScriptInterface<IInteractable>> &Interactables);

public:
	UFUNCTION(BlueprintPure, Category = "Interaction|Getter")
	EInteractionSearchMode GetMode() const { return DetectionMode; }
	UFUNCTION(BlueprintPure, Category = "Interaction|Getter")
	int GetMax() const { return MAXHITS; }

public:
	const TScriptInterface<IInteractable> GetInteractionTarget() const
	{
		if (TargetInteraction != nullptr)
		{
			return TargetInteraction->GetInterface(DetectionMode);
		}

		return nullptr;
	}

protected:
	const FInteractionRecord *GetInteractionRecord() const
	{
		return TargetInteraction;
	}
};
