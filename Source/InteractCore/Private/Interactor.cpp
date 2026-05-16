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
	bHasSelectInteraction = GetClass()->IsFunctionImplementedInScript(GET_FUNCTION_NAME_CHECKED(UInteractor, K2_SelectInteractionTarget));
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
			if (MultiHovering)
			{
				TargetInteraction = ChooseInteractionTarget(ActiveHoverData);
			}
			else
			{
				TargetInteraction = &ActiveHoverData[0];
			}
		}
	}
	else
	{
		if (ActiveHoverData.HasAnyRecord())
		{
			for (int32 j = 0; j < GetMax(); ++j)
			{
				if (ActiveHoverData[j].IsValid())
				{
					UObject *Obj;
					if (ActiveHoverData[j].TryGetInterface(DetectionMode, Obj))
					{
						IInteractable::Execute_UnHover(Obj, this);
					}
				}
			}
		}
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
bool UInteractor::EvaluateTraceHits(const FInteractionData &NewData, FInteractionData &OldData)
{
	// ----- Single hover mode (fast path) -----
	if (!MultiHovering)
	{
		if (NewData[0] == OldData[0])
			return false;

		UObject *Obj;

		if (OldData[0].TryGetInterface(DetectionMode, Obj))
		{
			IInteractable::Execute_UnHover(Obj, this);
		}

		if (NewData[0].TryGetInterface(DetectionMode, Obj))
		{
			IInteractable::Execute_Hover(Obj, this, NewData[0].GetHit());
		}

		OldData = NewData;
		return true;
	}

	const int32 Max = GetMax();
	bool bChanged = false;
	bool OldMatched[MAXHITS] = {false};
	// ----- Process new hits -----
	for (int32 i = 0; i < Max; ++i)
	{
		if (!NewData[i].IsValid())
			continue;

		int32 MatchIndex = -1;
		for (int32 j = 0; j < Max; ++j)
		{
			if (!OldMatched[j] && OldData[j].IsValid() && NewData[i] == OldData[j])
			{
				MatchIndex = j;
				break;
			}
		}

		// mark match
		if (MatchIndex >= 0)
		{
			OldMatched[MatchIndex] = true;
			continue;
		}

		// New hover
		UObject *Obj;
		if (NewData[i].TryGetInterface(DetectionMode, Obj))
		{
			IInteractable::Execute_Hover(Obj, this, NewData[i].GetHit());
		}

		bChanged = true;
	}
	// ----- Process unhovers -----
	for (int32 i = 0; i < Max; ++i)
	{
		if (!OldData[i].IsValid() || OldMatched[i])
			continue;

		UObject *Obj;
		if (OldData[i].TryGetInterface(DetectionMode, Obj))
		{
			IInteractable::Execute_UnHover(Obj, this);
		}

		bChanged = true;
	}

	OldData = NewData;
	return bChanged;
}
const FInteractionRecord *UInteractor::ChooseInteractionTarget(const FInteractionData &HoverData)
{
	if (bHasSelectInteraction)
	{
		TScriptInterface<IInteractable> SelectedInteractable =
			K2_SelectInteractionTarget(HoverData.GetInteractables(DetectionMode));

		return HoverData.FindRecordByInterface(SelectedInteractable.GetObject(), DetectionMode);
	}

	return SelectInteractionTarget(HoverData);
}
