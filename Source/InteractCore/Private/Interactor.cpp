// Fill out your copyright notice in the Description page of Project Settings.

#include "Interactor.h"

// Sets default values for this component's properties
UInteractor::UInteractor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	bHasSingleTrace = GetClass()->IsFunctionImplementedInScript(GET_FUNCTION_NAME_CHECKED(UInteractor, K2_PerformSingleTrace));
	bHasMultiTrace = GetClass()->IsFunctionImplementedInScript(GET_FUNCTION_NAME_CHECKED(UInteractor, K2_PerformMultiTrace));
}

// Called when the game starts
void UInteractor::BeginPlay()
{
	Super::BeginPlay();

	// ...
}

// Called every frame
void UInteractor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...

	CurrentDetectionData.Clear();
	if (PerformTrace(CurrentDetectionData))
	{
		if (EvaluateTraceHits(CurrentDetectionData, ActiveHoverData))
		{
			UpdateHoveredActors();
			int TargetIndex = ChooseInteractionTarget(ActiveHoverData);
		}
	}
	else
	{
		UpdateUnhoveredActors();
		return;
	}
}

bool UInteractor::PerformTrace(FInteractionData &DetectionData)
{
	if (MultiHovering)
	{
		if (bHasMultiTrace)
		{
			// Directly call BlueprintImplementableEvent
			TArray<FHitResult> Hits = K2_PerformMultiTrace();

			const int32 NumToCopy = FMath::Min(Hits.Num(), MAXHITS);

			for (int32 i = 0; i < NumToCopy; ++i)
			{
				DetectionData.SetHit(i, Hits[i]);
			}
			return NumToCopy > 0;
		}
		return PerformMultiTrace(DetectionData);
	}
	else
	{
		if (bHasSingleTrace)
		{
			// Directly call BlueprintImplementableEvent
			FHitResult Hit = K2_PerformSingleTrace();
			if (Hit.bBlockingHit)
			{
				DetectionData.SetHit(0, Hit);
				return true;
			}
			return false;
		}
		return PerformSingleTrace(DetectionData);
	}
}
bool UInteractor::EvaluateTraceHits(const FInteractionData &NewData, FInteractionData OldData)
{
	if (MultiHovering == false)
	{
		if (NewData[0] == OldData[0])
			return false;

		UObject *Obj;
		if (OldData[0].TryGetInterface(DetectionMode, Obj))
		{
			IInteractable::Execute_UnHover(Obj, this);
		}
	}

	bool bChanged = false;

	// 1. Check old hovered items
	// for (int i = 0; i < GetMax(); ++i)
	// {
	// 	auto &OldItem = HoverData.Data[i];

	// 	if (!OldItem.Interface)
	// 		continue;

	// 	bool bFound = false;

	// 	for (int j = 0; j < GetMax(); ++j)
	// 	{
	// 		if (DetectionData.Data[j].Interface == OldItem.Interface)
	// 		{
	// 			bFound = true;
	// 			break;
	// 		}
	// 	}

	// 	if (!bFound)
	// 	{
	// 		IInteractable::Execute_OnUnhover(
	// 			OldItem.Interface.GetObject());

	// 		OldItem.Interface = nullptr;
	// 		OldItem.bIsHovered = false;

	// 		bChanged = true;
	// 	}
	// }

	// // 2. Check new detected items
	// for (int i = 0; i < GetMax(); ++i)
	// {
	// 	auto &NewItem = DetectionData.Data[i];

	// 	if (!NewItem.Interface)
	// 		continue;

	// 	bool bFound = false;

	// 	for (int j = 0; j < GetMax(); ++j)
	// 	{
	// 		if (HoverData.Data[j].Interface == NewItem.Interface)
	// 		{
	// 			bFound = true;
	// 			break;
	// 		}
	// 	}

	// 	if (!bFound)
	// 	{
	// 		for (int k = 0; k < GetMax(); ++k)
	// 		{
	// 			if (!HoverData.Data[k].Interface)
	// 			{
	// 				HoverData.Data[k] = NewItem;

	// 				IInteractable::Execute_OnHover(
	// 					NewItem.Interface.GetObject());

	// 				bChanged = true;
	// 				break;
	// 			}
	// 		}
	// 	}
	// }

	return bChanged;
}
int UInteractor::ChooseInteractionTarget(const FInteractionData &HoverData)
{
	return 0;
}
