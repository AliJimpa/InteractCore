// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractorN.h"

// Sets default values for this component's properties
UInteractorN::UInteractorN()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	bHasBPTrace = GetClass()->IsFunctionImplementedInScript(GET_FUNCTION_NAME_CHECKED(UInteractorN, DoTrace));
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

bool UInteractorN::PerformTrace(FInteractionRecord &DetectionData)
{
	if (bHasBPTrace)
	{
		if (DoTrace(DetectionData))
		{
			// Compair & UnHover
		}
		else
		{
			// Just UnHover
		}
	}
	else
	{
		LOG("Nothing CPP");
	}
	return false;
}
bool UInteractorN::EvaluateTraceHits(const FInteractionRecord &DetectionData, FInteractionRecord HoverData)
{
	return false;
}
int UInteractorN::ChooseInteractionTarget(const FInteractionRecord &HoverData)
{
	return 0;
}
