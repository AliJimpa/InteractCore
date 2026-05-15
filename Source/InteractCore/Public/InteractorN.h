// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractDebug.h"
#include "InteractorN.generated.h"

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

struct FInteractionRecord
{
private:
	FHitResult Hit;
	TScriptInterface<IInteractable> Interface;
	FORCEINLINE void SetInterface(UObject *Obj)
	{
		if (!Obj)
			return;

		Interface.SetObject(Obj);
		Interface.SetInterface(Cast<IInteractable>(Obj));
	}

public:
	FORCEINLINE void SetHit(const FHitResult &InHit)
	{
		Hit = InHit;
		Interface = nullptr;
	}
	FORCEINLINE TScriptInterface<IInteractable> GetInterface(EInteractionSearchMode Mode)
	{
		if (Interface)
		{
			return Interface;
		}

		AActor *HitActor = Hit.GetActor();
		UActorComponent *HitComp = Hit.GetComponent();

		if (!HitActor && !HitComp)
		{
			return nullptr;
		}

		switch (Mode)
		{
		case EInteractionSearchMode::ActorOnly:
		{
			if (HitActor && HitActor->Implements<UInteractable>())
			{
				SetInterface(HitActor);
			}
			break;
		}

		case EInteractionSearchMode::ComponentOnly:
		{
			if (HitComp && HitComp->Implements<UInteractable>())
			{
				SetInterface(HitComp);
			}
			break;
		}

		case EInteractionSearchMode::ActorAndComponent:
		{
			if (HitActor && HitActor->Implements<UInteractable>())
			{
				SetInterface(HitActor);
			}
			else if (HitComp && HitComp->Implements<UInteractable>())
			{
				SetInterface(HitComp);
			}
			break;
		}

		case EInteractionSearchMode::PreferActorFallbackToComponent:
		{
			if (HitActor && HitActor->Implements<UInteractable>())
			{
				SetInterface(HitActor);
				break;
			}

			if (HitActor)
			{
				TArray<UActorComponent *> Components =
					HitActor->GetComponentsByInterface(UInteractable::StaticClass());

				if (Components.Num() > 0)
				{
					SetInterface(Components[0]);
				}
			}
			break;
		}
		}

		return Interface;
	}
	FORCEINLINE bool IsValid(EInteractionSearchMode Mode)
	{
		return GetInterface(Mode) != nullptr;
	}
};

struct FInteractionData
{
private:
	TStaticArray<FInteractionRecord, MAXHITS> Data;

	UPROPERTY()
	bool bHovered = false;
public:
	FORCEINLINE void SetHit(const FHitResult hit)
	{
		// if (bHovered == false)
		// {
		// 	Hit = hit;
		// }
		// else
		// {
		// 	LOG_ERROR("You can Add More");
		// 	PRINT("You can Add More");
		// }
	}

	FORCEINLINE void Clear()
	{
		LOG("Clear Record");
	}
};

UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class INTERACTCORE_API UInteractorN : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInteractorN();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual bool PerformTrace(FInteractionData &DetectionData) PURE_VIRTUAL(UInteractorN::PerformTrace, return false;);

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

private:
	bool bHasBPTrace = false;
	FInteractionData CurrentDetectionData;
	FInteractionData ActiveHoverData;

	bool EvaluateTraceHits(const FInteractionData &DetectionData, FInteractionData HoverData);
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

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interactor")
	EInteractionSearchMode DetectionMode;
	// UFUNCTION(BlueprintCallable, Category = "Interaction")
	// static void AddHIT(UPARAM(ref) FInteractionData &Buffer, const FHitResult hit)
	// {
	// 	Buffer.SetHit(hit);
	// }
	// UFUNCTION(BlueprintImplementableEvent, Category = "Interaction")
	// bool DoTrace(const FInteractionData &Data);

public:
	UFUNCTION(BlueprintPure, Category = "Interactor")
	EInteractionSearchMode GetMode() const { return DetectionMode; }
	UFUNCTION(BlueprintPure, Category = "Interactor")
	int GetMax() const { return MAXHITS; }
};
