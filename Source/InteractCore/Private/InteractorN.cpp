// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractorN.h"

// Sets default values for this component's properties
UInteractorN::UInteractorN()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	bHasSingleTrace = GetClass()->IsFunctionImplementedInScript(GET_FUNCTION_NAME_CHECKED(UInteractorN, K2_PerformSingleTrace));
	bHasMultiTrace = GetClass()->IsFunctionImplementedInScript(GET_FUNCTION_NAME_CHECKED(UInteractorN, K2_PerformMultiTrace));
}

// Called when the game starts
void UInteractorN::BeginPlay()
{
	Super::BeginPlay();

	// ...
}

// Called every frame
void UInteractorN::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
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

bool UInteractorN::PerformTrace(FInteractionData &DetectionData)
{
	if (MultiHovering)
	{
		if (bHasMultiTrace)
		{
			TArray<FHitResult> Hits = K2_PerformMultiTrace();

			const int32 NumToCopy = FMath::Min(Hits.Num(), MAXHITS);

			for (int32 i = 0; i < NumToCopy; ++i)
			{
				//DetectionData[i].SetHit(Hits[i]);
			}

			// Clear the unused entries
			for (int32 i = NumToCopy; i < MAXHITS; ++i)
			{
				//DetectionData[i].Clear();
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
				//DetectionData[0].SetHit(Hit);
				return true;
			}
			return false;
		}
		return PerformSingleTrace(DetectionData);
	}
}
bool UInteractorN::EvaluateTraceHits(const FInteractionData &DetectionData, FInteractionData HoverData)
{
	// if (MultiHovering == false)
	// {

	// 	return;
	// }

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
int UInteractorN::ChooseInteractionTarget(const FInteractionData &HoverData)
{
	return 0;
}
