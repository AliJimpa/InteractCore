// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseInteractor.h"
#include "InteractDebug.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/PlayerController.h"
#include "Interactable.h"

// Sets default values for this component's properties
UBaseInteractor::UBaseInteractor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UBaseInteractor::BeginPlay()
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

			if (InteractionInput != nullptr)
			{
				EIC->BindAction(InteractionInput, ETriggerEvent::Triggered, this, &UInteractor::OnInteractInput_Triggered);
				EIC->BindAction(InteractionInput, ETriggerEvent::Started, this, &UInteractor::OnInteractInput_Started);
				EIC->BindAction(InteractionInput, ETriggerEvent::Ongoing, this, &UInteractor::OnInteractInput_Ongoing);
				EIC->BindAction(InteractionInput, ETriggerEvent::Canceled, this, &UInteractor::OnInteractInput_Canceled);
				EIC->BindAction(InteractionInput, ETriggerEvent::Completed, this, &UInteractor::OnInteractInput_Completed);
			}
			else
			{
				LOG_ERROR("Interaction InteractionInput is not valid !");
				return;
			}
		}
	}
	else
	{
		LOG_ERROR("The Owner Actor [%s] is not Ownership to get controller", *GetOwner()->GetName());
	}
}

// Called every frame
void UBaseInteractor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
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

	CompaireRecords(DetectionData, HoverData);

	if (!TrySortStage(DetectionData))
	{
		LOG_ERROR("TrySort Failed!");
		return;
	}

	CheckHoverStage(DetectionData);
}

AController *UBaseInteractor::ResolveControllerFromOwnership() const
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

void UBaseInteractor::OnInteractInput_Triggered(const FInputActionInstance &Instance)
{
	OnInteractTrigger(ETriggerEvent::Triggered);
}
void UBaseInteractor::OnInteractInput_Started(const FInputActionInstance &Instance)
{
	OnInteractTrigger(ETriggerEvent::Started);
}
void UBaseInteractor::OnInteractInput_Ongoing(const FInputActionInstance &Instance)
{
	OnInteractTrigger(ETriggerEvent::Ongoing);
}
void UBaseInteractor::OnInteractInput_Canceled(const FInputActionInstance &Instance)
{
	OnInteractTrigger(ETriggerEvent::Canceled);
}
void UBaseInteractor::OnInteractInput_Completed(const FInputActionInstance &Instance)
{
	OnInteractTrigger(ETriggerEvent::Completed);
}
