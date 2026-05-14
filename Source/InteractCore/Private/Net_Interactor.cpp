// Fill out your copyright notice in the Description page of Project Settings.

#include "Net_Interactor.h"
#include "InteractDebug.h"

// Called when the game starts
void UNet_Interactor::BeginPlay()
{
	Super::BeginPlay();

	// ...
}

void UNet_Interactor::InputBP_Implementation(int id)
{
	LOG_WARNING("InputBP_Implementation Called");
}

// bool UNet_Interactor::CanUpdateInteraction_Implementation() const
// {
// 	return true;
// }
// TArray<FHitResult> UInteractor::DetectInteractables_Implementation(TArray<TScriptInterface<IInteractable>> &Interactables)
// {
// 	// if (!OtherActor)
// 	// 	return;

// 	// if (OtherActor->GetClass()->ImplementsInterface(UWeightInterace::StaticClass()))
// 	// {
// 	// 	// Call the BlueprintNativeEvent
// 	// 	float TargetWeight = IWeightInterace::Execute_GetWeight(OtherActor);
// 	// 	SetTotalWeight(TotalWeight + TargetWeight);
// 	// }

// 	// if (CheckWight())
// 	// {
// 	// 	SetEnablePlate(true);
// 	// }
// 	return {};
// }
// bool UNet_Interactor::FilterHoverInteractables_Implementation(TArray<TScriptInterface<IInteractable>> &Interactables)
// {
// 	return true;
// }
// TArray<FHitResult> Hits = DetectInteractables(DetectedInteractables);
// if (DetectedInteractables.IsEmpty())
// {
// 	// LOG("List is Empty");
// 	return;
// }
// if (!FilterHoverInteractables(DetectedInteractables))
// {
// 	LOG_ERROR("FilterHoverInteractables Failed!");
// 	return;
// }

// HoverdInteractables.Reset();
// for (const TScriptInterface<IInteractable> &Interactable : DetectedInteractables)
// {
// 	UObject *Obj = Interactable.GetObject();
// 	if (Obj != nullptr)
// 	{
// 		if (IInteractable::Execute_CanHover(Obj))
// 		{
// 			IInteractable::Execute_Hover(Obj, this, FHitResult::FHitResult());
// 			HoverdInteractables.Add(Interactable);
// 		}
// 	}
// 	else
// 	{
// 		LOG_WARNING("Find invalid Interactable in Detection List");
// 	}
// }