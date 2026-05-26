// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "Interactable.h"
#include "SphereInteractable.generated.h"
/**
 *
 */
UCLASS(Blueprintable)
class INTERACTCORE_API USphereInteractable : public USphereComponent, public IInteractable
{
	GENERATED_BODY()

protected:
	// Implement IInteraction Interface
	virtual void Hover_Implementation(UActorComponent *Provider, FHitResult Hit) override;
	virtual void UnHover_Implementation(UActorComponent *Provider) override;

};
