// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractDebug.h"
#include "Interactable.h"
#include "Interactor.generated.h"

// The total value can allocate in stack for usein this component
#define MAXHITS 5

UENUM()
enum class EInteractionSearchMode : uint8
{
	ActorOnly UMETA(
		DisplayName = "Actor Only",
		ToolTip = "Only checks the hit Actor for the interaction interface."),

	ComponentOnly UMETA(
		DisplayName = "Component Only",
		ToolTip = "Only checks the hit Components for the interaction interface."),

	ActorAndComponent UMETA(
		DisplayName = "Actor And Component",
		ToolTip = "Checks both the hit Actor and its Components for the interaction interface."),

	PreferActorFallbackToComponent UMETA(
		DisplayName = "Prefer Actor, Fallback To Component",
		ToolTip = "Checks the Actor first. If the Actor does not implement the interface, searches Components for a valid implementation.")
};

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
		Hit = InHit;
		bValid = Hit.bBlockingHit;
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
	FORCEINLINE const FInteractionRecord &operator[](int32 Index) const
	{
		return Data[Index]; // returns by const reference
	}
	FORCEINLINE void SetHit(int32 index, const FHitResult &InHit)
	{
		Data[index].SetHit(InHit);
	}
	FORCEINLINE void Clear()
	{
		for (int32 Index = 0; Index < MAXHITS; ++Index)
		{
			Data[Index].Clear();
		}
	}
};

UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
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

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

private:
	bool bHasSingleTrace = false;
	bool bHasMultiTrace = false;
	FInteractionData CurrentDetectionData;
	FInteractionData ActiveHoverData;
	bool PerformTrace(FInteractionData &DetectionData);
	bool EvaluateTraceHits(const FInteractionData &NewData, FInteractionData OldData);
	int ChooseInteractionTarget(const FInteractionData &HoverData);
	void UpdateHoveredActors()
	{
		// if(CanHover())
		// {

		// }
	}
	void UpdateUnhoveredActors()
	{
		///
	}

private:
	UPROPERTY(EditAnywhere, Category = "Interactor|Input")
	bool MultiHovering = false;
	UPROPERTY(VisibleAnywhere, Category = "Interactor|Input", meta = (EditCondition = "MultiHovering == true", EditConditionHides))
	int MaxHovering;
	UPROPERTY(EditAnywhere, Category = "Interactor")
	EInteractionSearchMode DetectionMode = EInteractionSearchMode::ActorAndComponent;

protected:
	// UFUNCTION(BlueprintCallable, Category = "Interaction")
	// static void AddHIT(UPARAM(ref) FInteractionData &Buffer, const FHitResult hit)
	// {
	// 	Buffer.SetHit(hit);
	// }
	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction")
	FHitResult K2_PerformSingleTrace();
	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction")
	TArray<FHitResult> K2_PerformMultiTrace();

public:
	UFUNCTION(BlueprintPure, Category = "Interactor")
	EInteractionSearchMode GetMode() const { return DetectionMode; }
	UFUNCTION(BlueprintPure, Category = "Interactor")
	int GetMax() const { return MAXHITS; }
};
