// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractionComponent.h"
#include "Interactable.h"
#include "EnhancedInputComponent.h"

// Sets default values for this component's properties
UInteractionComponent::UInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	bIsImp_AdaptiveTick = GetClass()->IsFunctionImplementedInScript(GET_FUNCTION_NAME_CHECKED(UInteractionComponent, K2_TryUpdateAdaptiveTick));
	bIsImp_PivotComp = GetClass()->IsFunctionImplementedInScript(GET_FUNCTION_NAME_CHECKED(UInteractionComponent, K2_GetInteractionPivotLocation));
}

// Called when the game starts
void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	AController *Controller = ResolveControllerFromOwnership();

	CacheCameraManager(Controller);

	if (bInteractionInputBound)
	{
		SetupInteractionInput(Controller);
	}
}

// Called every frame
void UInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	if (bAdaptiveInterval)
	{
		float NewRate;
		if (bIsImp_AdaptiveTick)
		{
			if (K2_TryUpdateAdaptiveTick(AdaptiveIntervalThreshold, NewRate))
			{
				PrimaryComponentTick.TickInterval = 1.f / NewRate;
			}
		}
		else
		{
			if (TryUpdateAdaptiveTick(AdaptiveIntervalThreshold, NewRate))
			{
				PrimaryComponentTick.TickInterval = 1.f / NewRate;
			}
		}
	}

	PreHovering();
	UpdateInteraction();
	PostHovering();
}

// Pivot Component
FVector UInteractionComponent::GetPivot() const
{
	if (bIsImp_PivotComp)
	{
		return K2_GetInteractionPivotLocation();
	}
	else
	{
		return GetInteractionPivotLocation();
	}
}

void UInteractionComponent::CacheCameraManager(AController *Controller)
{
	if (CameraManager)
		return;

	if (!Controller)
	{
		LOG_ERROR("The Owner Actor [%s] is not Ownership to get controller", *GetOwner()->GetName());
		return;
	}

	APlayerController *PC = Cast<APlayerController>(Controller);
	if (!PC)
	{
		LOG_WARNING("Pawn has no PlayerController yet");
		return;
	}

	CameraManager = PC->PlayerCameraManager;
}
FVector UInteractionComponent::GetInteractionPivotLocation() const
{
	if (!CameraManager)
		return FVector::ZeroVector;
	return CameraManager->GetCameraLocation();
}

// TickInterval
bool UInteractionComponent::TryUpdateAdaptiveTick(float Threshould, float &OutTickRate)
{
	OutTickRate = 0.033f;
	if (!CameraManager)
		return false;

	const FRotator CameraRot = CameraManager->GetCameraRotation();

	const float Delta = CameraRot.GetManhattanDistance(LastCameraRotation);

	if (Delta > Threshould)
	{
		OutTickRate = 60;
	}
	else
	{
		OutTickRate = 10;
	}

	LastCameraRotation = CameraRot;
	return true;
}

// Core Interaction Logic
void UInteractionComponent::UpdateInteraction()
{
	FHitResult DetectedFocused;
	TScriptInterface<IInteractable> NewFocused;
	if (TryGetDetectedFocused(DetectedFocused))
	{
		// valid hit
		NewFocused = ResolveInteractableFromHit(DetectedFocused);

		// UpdateNewFocused
		if (NewFocused != CurrentFocused)
		{
			if (CurrentFocused)
			{
				IInteractable::Execute_UnHover(CurrentFocused.GetObject(), this);
			}
			if (NewFocused)
			{
				IInteractable::Execute_Hover(NewFocused.GetObject(), this, DetectedFocused);
			}
			CurrentFocused = NewFocused;
		}
	}
	else
	{
		if (CurrentFocused != nullptr)
		{
			IInteractable::Execute_UnHover(CurrentFocused.GetObject(), this);
		}
	}
}
TScriptInterface<IInteractable> UInteractionComponent::ResolveInteractableFromHit(const FHitResult &Hit) const
{
	AActor *HitActor = Hit.GetActor();
	UActorComponent *HitComp = Hit.GetComponent();

	auto MakeInteractableInterface = [](UObject *Object) -> TScriptInterface<IInteractable>
	{
		TScriptInterface<IInteractable> Result;

		if (Object && Object->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
		{
			Result.SetObject(Object);
			Result.SetInterface(Cast<IInteractable>(Object));
		}

		return Result;
	};

	switch (DetectionMode)
	{
	case EInteractionSearchMode::ActorOnly:
		return MakeInteractableInterface(HitActor);

	case EInteractionSearchMode::ComponentOnly:
		return MakeInteractableInterface(HitComp);

	case EInteractionSearchMode::ActorAndComponent:
	{
		TScriptInterface<IInteractable> Result = MakeInteractableInterface(HitActor);
		if (Result)
		{
			return Result;
		}

		return MakeInteractableInterface(HitComp);
	}

	case EInteractionSearchMode::PreferActorFallbackToComponent:
	{
		TScriptInterface<IInteractable> Result = MakeInteractableInterface(HitActor);
		if (Result)
		{
			return Result;
		}

		if (HitActor)
		{
			TInlineComponentArray<UActorComponent *> Components(HitActor);
			for (UActorComponent *Comp : Components)
			{
				Result = MakeInteractableInterface(Comp);
				if (Result)
				{
					return Result;
				}
			}
		}

		return nullptr;
	}
	}

	return nullptr;
}

// Inputs
void UInteractionComponent::SetupInteractionInput(AController *Controller)
{
	if (!Controller)
	{
		LOG_ERROR("The Owner Actor [%s] is not Ownership to get controller", *GetOwner()->GetName());
		return;
	}

	if (!Controller->IsLocalController())
	{
		return;
	}

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

	if (!InteractionInput)
	{
		LOG_ERROR("InteractionInput is not valid!");
		return;
	}

	BindInteractionInput(EIC);
}
AController *UInteractionComponent::ResolveControllerFromOwnership() const
{
	if (APawn *Pawn = Cast<APawn>(GetOwner()))
	{
		return Pawn->GetController();
	}

	if (AController *Controller = Cast<AController>(GetOwner()))
	{
		return Controller;
	}

	return nullptr;
}
void UInteractionComponent::BindInteractionInput(UEnhancedInputComponent *EIC)
{
	EIC->BindAction(InteractionInput, ETriggerEvent::Started, this, &UInteractionComponent::OnInteractInput);
	EIC->BindAction(InteractionInput, ETriggerEvent::Triggered, this, &UInteractionComponent::OnInteractInput);
	EIC->BindAction(InteractionInput, ETriggerEvent::Ongoing, this, &UInteractionComponent::OnInteractInput);
	EIC->BindAction(InteractionInput, ETriggerEvent::Canceled, this, &UInteractionComponent::OnInteractInput);
	EIC->BindAction(InteractionInput, ETriggerEvent::Completed, this, &UInteractionComponent::OnInteractInput);
}
void UInteractionComponent::OnInteractInput(const FInputActionInstance &Instance)
{
	switch (Instance.GetTriggerEvent())
	{
	case ETriggerEvent::Started:
		// HandleInteractStarted();
		break;

	case ETriggerEvent::Triggered:
		// HandleInteractTriggered();
		IInteractable::Execute_Interact(GetCurrentFocused().GetObject(), this);
		break;

	case ETriggerEvent::Ongoing:
		// HandleInteractOngoing();
		break;

	case ETriggerEvent::Canceled:
		// HandleInteractCanceled();
		break;

	case ETriggerEvent::Completed:
		// HandleInteractCompleted();
		break;

	default:
		break;
	}
}
