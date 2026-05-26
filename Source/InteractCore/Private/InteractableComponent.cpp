// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractableComponent.h"
#include "EnhancedInput\Public\InputTriggers.h"


void UInteractableComponent::Interact_Implementation(UActorComponent *Provider)
{
}
void UInteractableComponent::Hover_Implementation(UActorComponent *Provider, FHitResult Hit)
{
}
void UInteractableComponent::UnHover_Implementation(UActorComponent *Provider)
{
}
// bool UInteractableComponent::CanInteract_Implementation(ETriggerEvent InputEventType, const FInputActionInstance &InputValue) const
// {
//     if (InputEventType == ETriggerEvent::Started)
//     {
//         return true;
//     }

//     return false;
// }