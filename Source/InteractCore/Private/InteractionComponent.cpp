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
}

// Called when the game starts
void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	AController *Controller = ResolveControllerFromOwnership();
	if (bInteractionInputBound)
	{
		SetupInteractionInput(Controller);
	}
	OnControllerReady(Controller);
}

// Called every frame
void UInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	UpdateInteraction();
}

// Functions
void UInteractionComponent::SetInteractionActive(bool bEnable)
{
	if (bEnable)
	{
		Activate();
		SetComponentTickEnabled(true);
	}
	else
	{
		Deactivate();
		SetComponentTickEnabled(false);
	}
}
FTransform UInteractionComponent::GetPivot() const
{
	if (PivotComponent)
	{
		return PivotComponent->GetComponentTransform();
	}

	return PivotValue;
}
void UInteractionComponent::SetPivotToComponent(USceneComponent *InComponent)
{
	PivotComponent = InComponent;
}
void UInteractionComponent::SetPivotToTransform(const FTransform &InValue)
{
	PivotComponent = nullptr;
	PivotValue = InValue;
}

// Core Interaction Logic
void UInteractionComponent::UpdateInteraction()
{
	FHitResult HitResult;
	TScriptInterface<IInteractable> NewFocused = nullptr;

	if (TryGetDetectedFocused(HitResult))
	{
		UObject *CurrentObject = CurrentInteractable.GetObject();
		if (CurrentObject)
		{
			if (HitResult.GetActor() == CurrentObject || HitResult.GetComponent() == CurrentObject)
			{
				return;
			}
		}

		NewFocused = ResolveInteractableFromHit(HitResult);
	}


	if (NewFocused.GetObject() != CurrentInteractable.GetObject())
	{
		SetCurrentInteractable(NewFocused, HitResult);
	}
}
void UInteractionComponent::SetCurrentInteractable(const TScriptInterface<IInteractable> &NewInteractable, const FHitResult &HitResult)
{
	if (UObject *CurrentObject = CurrentInteractable.GetObject())
	{
		IInteractable::Execute_UnHover(CurrentObject, this);
		//LOG_WARNING("UnHover");
	}

	CurrentInteractable = nullptr;

	if (UObject *NewObject = NewInteractable.GetObject())
	{
		if (CanHover(NewObject))
		{
			CurrentInteractable = NewInteractable;
			IInteractable::Execute_Hover(CurrentInteractable.GetObject(), this, HitResult);
			//LOG_WARNING("Hover");
		}
		else
		{
			//LOG_WARNING("Can't Hover");
		}
	}
}
TScriptInterface<IInteractable> UInteractionComponent::ResolveInteractableFromHit(const FHitResult &Hit) const
{
	AActor *HitActor = Hit.GetActor();
	UActorComponent *HitComp = Hit.GetComponent();
	static const UClass *InteractableClass = UInteractable::StaticClass();

	auto MakeInteractableInterface = [](UObject *Object) -> TScriptInterface<IInteractable>
	{
		TScriptInterface<IInteractable> Result = nullptr;

		if (!Object)
		{
			return Result;
		}

		if (Object->GetClass()->ImplementsInterface(InteractableClass))
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
		auto Result = MakeInteractableInterface(HitActor);
		if (Result)
			return Result;

		if (HitActor)
		{
			TInlineComponentArray<UActorComponent *> Components(HitActor);
			for (UActorComponent *Comp : Components)
			{
				Result = MakeInteractableInterface(Comp);
				if (Result)
					return Result;
			}
		}
		return TScriptInterface<IInteractable>();
	}

	default:
		return TScriptInterface<IInteractable>();
	}
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
		LOG_ERROR("Controller should be LocalController");
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
		IInteractable::Execute_Interact(GetCurrentInteractable().GetObject(), this);
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
