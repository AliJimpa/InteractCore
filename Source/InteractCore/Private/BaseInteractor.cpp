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

// Called every frame
void UBaseInteractor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
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
void UBaseInteractor::OnTriggered(const FInputActionInstance &Instance)
{
	OnInteractionCalled(ETriggerEvent::Triggered);
}
void UBaseInteractor::OnStarted(const FInputActionInstance &Instance)
{
	OnInteractionCalled(ETriggerEvent::Started);
}
void UBaseInteractor::OnOngoing(const FInputActionInstance &Instance)
{
	OnInteractionCalled(ETriggerEvent::Ongoing);
}
void UBaseInteractor::OnCanceled(const FInputActionInstance &Instance)
{
	OnInteractionCalled(ETriggerEvent::Canceled);
}
void UBaseInteractor::OnCompleted(const FInputActionInstance &Instance)
{
	OnInteractionCalled(ETriggerEvent::Completed);
}
