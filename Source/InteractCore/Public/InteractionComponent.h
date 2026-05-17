// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractDebug.h"
#include "InteractionComponent.generated.h"

class IInteractable;
class UInputAction;
struct FInputActionInstance;
class UEnhancedInputComponent;

UENUM()
enum class EInteractionSearchMode : uint8
{
	ActorOnly UMETA(
		DisplayName = "Actor Only",
		ToolTip = "Only checks the hit Actor for the interaction interface."),

	ComponentOnly UMETA(
		DisplayName = "Component Only",
		ToolTip = "Only checks the hit Components for the interaction interface."),

	ActorAndComponent UMETA(
		DisplayName = "Actor And Component",
		ToolTip = "Checks both the hit Actor and its Components for the interaction interface."),

	PreferActorFallbackToComponent UMETA(
		DisplayName = "Prefer Actor, Fallback To Component",
		ToolTip = "Checks the Actor first. If the Actor does not implement the interface, searches Components for a valid implementation.")
};

UCLASS(Abstract)
class INTERACTCORE_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInteractionComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual bool GetDetectedFocused(FHitResult &OutHit) const PURE_VIRTUAL(UInteractionComponent::GetDetectedFocused, return false;);

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

private:
	TScriptInterface<IInteractable> CurrentFocused = nullptr;
	TScriptInterface<IInteractable> ResolveInteractableFromHit(const FHitResult &Hit) const;
	AController *ResolveControllerFromOwnership() const
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
	void SetupInteractionInput();
	void BindInteractionInput(UEnhancedInputComponent *EIC);
	void OnInteractInput(const FInputActionInstance &Instance);

private:
	UPROPERTY(EditAnywhere, Category = "Interaction")
	EInteractionSearchMode DetectionMode = EInteractionSearchMode::ActorAndComponent;
	UPROPERTY(EditAnywhere, Category = "Interaction|Input")
	bool bInteractionInputBound = true;
	UPROPERTY(EditAnywhere, Category = "Interaction|Input")
	UInputAction *InteractionInput;
};
