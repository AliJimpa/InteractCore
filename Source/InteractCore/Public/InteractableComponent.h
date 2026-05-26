// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "Interactable.h"
#include "InteractableComponent.generated.h"

UENUM(BlueprintType)
enum class EInputInteractionType : uint8
{
	Press UMETA(DisplayName = "Press"),
	Release UMETA(DisplayName = "Release"),
	Tap UMETA(DisplayName = "Tap"),
	Hold UMETA(DisplayName = "Hold"),
	DoubleClick UMETA(DisplayName = "Double Click"),
	Any UMETA(DisplayName = "Any")
};

/**
 *
 */
UCLASS(Blueprintable)
class INTERACTCORE_API UInteractableComponent : public USphereComponent, public IInteractable
{
	GENERATED_BODY()

protected:
	// Implement IInteraction Interface
	virtual void Interact_Implementation(UActorComponent *Provider) override;
	virtual void Hover_Implementation(UActorComponent *Provider, FHitResult Hit) override;
	virtual void UnHover_Implementation(UActorComponent *Provider) override;
};
