// Fill out your copyright notice in the Description page of Project Settings.

#include "Interactor.h"
#include "InteractDebug.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/PlayerController.h"
#include "Interactable.h"

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

	if (AController *Controller = ResolveControllerFromOwnership())
	{
		if (Controller->IsLocalController())
		{
			APlayerController *PC = Cast<APlayerController>(Controller);
			if (!PC)
			{
				LOG_WARNING("Pawn has no PlayerController yet");
				return;
			}

			UEnhancedInputComponent *EIC = Cast<UEnhancedInputComponent>(PC->InputComponent);
			if (!EIC)
			{
				LOG_WARNING("No EnhancedInputComponent on PlayerController");
				return;
			}

			if (InputAction != nullptr)
			{
				EIC->BindAction(InputAction, ETriggerEvent::Triggered, this, &UInteractor::OnTriggered);
				EIC->BindAction(InputAction, ETriggerEvent::Started, this, &UInteractor::OnStarted);
				EIC->BindAction(InputAction, ETriggerEvent::Ongoing, this, &UInteractor::OnOngoing);
				EIC->BindAction(InputAction, ETriggerEvent::Canceled, this, &UInteractor::OnCanceled);
				EIC->BindAction(InputAction, ETriggerEvent::Completed, this, &UInteractor::OnCompleted);
			}
			else
			{
				LOG_ERROR("Interaction InputAction is not valid !");
				return;
			}
		}
	}
	else
	{
		LOG_ERROR("The Owner Actor [%s] is not Ownership to get controller", *GetOwner()->GetName());
	}
}
void UInteractor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

// Called every frame
void UInteractor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	if (!CanUpdateInteraction())
	{
		CheckUnhoverStage();
		return;
	}

	// Reset
	for (FInteractTickRecord &Record : DetectionData)
	{
		Record.active = false;
	}

	if (!TryDetectStage(DetectionData))
	{
		LOG_ERROR("DetectStage Failed!");
		return;
	}

	if (!DoesAnyRecordDetect(DetectionData))
	{
		CheckUnhoverStage();
		return;
	}


	CompaireRecords(DetectionData , HoverData);


	if (!TrySortStage(DetectionData))
	{
		LOG_ERROR("TrySort Failed!");
		return;
	}

	CheckHoverStage(DetectionData);

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

AController *UInteractor::ResolveControllerFromOwnership() const
{
	AActor *Owner = GetOwner();

	int32 Depth = 0;
	while (Owner)
	{
		if (APawn *Pawn = Cast<APawn>(Owner))
		{
			return Pawn->GetController();
		}

		if (AController *Controller = Cast<AController>(Owner))
		{
			return Controller;
		}

		LOG("[%d] %s", Depth++, *Owner->GetName());
		Owner = Owner->GetOwner();
	}

	return nullptr;
}
void UInteractor::OnTriggered(const FInputActionInstance &Instance)
{
	InputBP(0);
}
void UInteractor::OnStarted(const FInputActionInstance &Instance)
{
	InputBP(1);
}
void UInteractor::OnOngoing(const FInputActionInstance &Instance)
{
	InputBP(2);
}
void UInteractor::OnCanceled(const FInputActionInstance &Instance)
{
	InputBP(3);
}
void UInteractor::OnCompleted(const FInputActionInstance &Instance)
{
	InputBP(4);
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