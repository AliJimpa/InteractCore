// Fill out your copyright notice in the Description page of Project Settings.

#include "Interactor.h"

// Sets default values for this component's properties
UInteractor::UInteractor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
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
}



// bool UInteractor::CanUpdateInteraction_Implementation() const
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
// bool UInteractor::FilterHoverInteractables_Implementation(TArray<TScriptInterface<IInteractable>> &Interactables)
// {
// 	return true;
// }