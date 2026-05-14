// Fill out your copyright notice in the Description page of Project Settings.

#include "Interactor.h"
#include "InteractDebug.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/PlayerController.h"
#include "Interactable.h"
#include "EnhancedPlayerInput.h"

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
				LOG_ERROR("InteractionInput is not valid!");
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
void UInteractor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	if (!CanUpdateInteraction(HoverInput != nullptr))
	{
		CheckUnhoverStage(HoverData);
		return;
	}

	// Reset
	for (int32 i = 0; i < DetectionData.Num(); ++i)
	{
		DetectionData[i].active = false;
	}

	if (!DetectionStage(DetectionData))
	{
		return;
	}

	if (!HasAnyActive(DetectionData))
	{
		CheckUnhoverStage(HoverData);
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

// Get owner control with trace ownership
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

// Binding methods for interactionInput
void UInteractor::OnInteractInput_Triggered(const FInputActionInstance &Instance)
{
	OnInteractTrigger(Instance, ETriggerEvent::Triggered);
}
void UInteractor::OnInteractInput_Started(const FInputActionInstance &Instance)
{
	OnInteractTrigger(Instance, ETriggerEvent::Started);
}
void UInteractor::OnInteractInput_Ongoing(const FInputActionInstance &Instance)
{
	OnInteractTrigger(Instance, ETriggerEvent::Ongoing);
}
void UInteractor::OnInteractInput_Canceled(const FInputActionInstance &Instance)
{
	OnInteractTrigger(Instance, ETriggerEvent::Canceled);
}
void UInteractor::OnInteractInput_Completed(const FInputActionInstance &Instance)
{
	OnInteractTrigger(Instance, ETriggerEvent::Completed);
}

void UInteractor::OnInteractTrigger(const FInputActionInstance &Instance, ETriggerEvent TriggerMode)
{
}

bool UInteractor::CanUpdateInteraction(bool UseInput) const
{
	// flip flop logic

	if (UseInput)
	{
		if (HoverInput != nullptr)
		{
			return IsHoverInputPressed();
		}
		else
		{
			return false;
		}
	}

	return true;
}
void UInteractor::CheckUnhoverStage(FInteractionBuffer &Buffer)
{
	for (FInteractionData2 &Element : HoverData)
	{
		if (!Element.active)
		{
			continue;
		}

		// UnHover(Element);
	}
}
void UInteractor::CheckHoverStage(FInteractionBuffer &Buffer)
{
	for (FInteractionData2 &Detection : DetectionData)
	{
		if (!Detection.active)
		{
			continue;
		}
		Hover(Detection);
		// Add to Hover Data
	}
}
bool UInteractor::DetectionStage(FInteractionBuffer &Buffer)
{

	return false;
}

bool UInteractor::IsHoverInputPressed() const
{
	if (!HoverInput)
	{
		LOG_ERROR("HoverInput is not valid!");
		return false;
	}

	if (AController *Controller = ResolveControllerFromOwnership())
	{
		if (Controller->IsLocalController())
		{
			APlayerController *PC = Cast<APlayerController>(Controller);
			if (!PC)
			{
				LOG_WARNING("Pawn has no PlayerController yet");
				return false;
			}

			const UEnhancedPlayerInput *EPI = Cast<UEnhancedPlayerInput>(PC->PlayerInput);
			if (!EPI)
			{
				LOG_WARNING("APlayerController has not any EnhancePlayerInput");
				return false;
			}

			return EPI->GetActionValue(HoverInput).Get<bool>();
		}
		else
		{
			LOG("TEST");
			return false;
		}
	}
	else
	{
		LOG_ERROR("The Owner Actor [%s] is not Ownership to get controller", *GetOwner()->GetName());
		return false;
	}
}