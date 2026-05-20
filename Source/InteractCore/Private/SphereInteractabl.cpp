// Fill out your copyright notice in the Description page of Project Settings.

#include "SphereInteractable.h"
#include "EnhancedInput\Public\InputTriggers.h"


// Implement IInteraction Interface
void USphereInteractable::Hover_Implementation(UActorComponent *Provider, FHitResult Hit)
{
}
void USphereInteractable::UnHover_Implementation(UActorComponent *Provider)
{
}
// bool USphereInteractable::CanInteract_Implementation(ETriggerEvent InputEventType, const FInputActionInstance &InputValue) const
// {
//     if (InputEventType == ETriggerEvent::Started)
//     {
//         return true;
//     }

//     return false;
// }