// Fill out your copyright notice in the Description page of Project Settings.

#include "SphereInteractable.h"
#include "EnhancedInput\Public\InputTriggers.h"


// Implement IInteraction Interface
void USphereInteractable::Hover_Implementation(UInteractor *Provider, FHitResult Hit)
{
}
void USphereInteractable::UnHover_Implementation(UInteractor *Provider)
{
}
void USphereInteractable::Interact_Implementation(UInteractor *Provider, FHitResult Hit)
{
}
bool USphereInteractable::CanHover_Implementation() const
{
    return false;
}
bool USphereInteractable::CanInteract_Implementation(ETriggerEvent InputEventType, const FInputActionInstance &InputValue) const
{
    if (InputEventType == ETriggerEvent::Started)
    {
        return true;
    }

    return false;
}