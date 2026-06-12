// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractableComponent.h"
#include "EnhancedInput\Public\InputTriggers.h"

UInteractableComponent::UInteractableComponent()
{
    SphereRadius = 32.0f;
    SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    SetCollisionResponseToAllChannels(ECR_Ignore);
    SetCollisionResponseToChannel(InteractChannel, ECR_Block);
    // RINT("Base");
}

void UInteractableComponent::OnRegister()
{
    Super::OnRegister();

    SetCollisionResponseToChannel(InteractChannel, ECR_Block);
}

void UInteractableComponent::Interact_Implementation(UInteractionComponent *Provider)
{
    if (InteractMode != EInteractionUsageMode::Multi)
    {
        bHasInteracted = true;
    }
    if (InteractMode == EInteractionUsageMode::Cooldown)
    {
        const UWorld *World = GetWorld();
        if (World != nullptr)
            NextAllowedInteractTime = World->GetTimeSeconds() + CooldowDuration;
    }
    OnInteract.Broadcast(Provider);
    // LOG("INTERACT");
}
void UInteractableComponent::Hover_Implementation(UInteractionComponent *Provider, FHitResult Hit)
{
    bIsHovered = true;
    OnHoverBegin.Broadcast(Provider);
    // LOG("HOVER");
}
void UInteractableComponent::UnHover_Implementation(UInteractionComponent *Provider)
{
    bIsHovered = false;
    OnHoverEnd.Broadcast(Provider);
    // LOG("UNHOVER");
}
bool UInteractableComponent::ShouldHandleInput_Implementation(const FInputActionInstance &InputValue) const
{
    if (!CanInteract())
        return false;

    const ETriggerEvent TriggerEvent = InputValue.GetTriggerEvent();

    switch (InputMode)
    {
    case EInteractionInputMode::Press:
    {
        return TriggerEvent == ETriggerEvent::Started;
    }
    case EInteractionInputMode::Release:
    {
        return TriggerEvent == ETriggerEvent::Completed;
    }
    case EInteractionInputMode::Tap:
    {
        // Best if Input Action has a Tap trigger configured
        return TriggerEvent == ETriggerEvent::Triggered;
    }
    case EInteractionInputMode::Hold:
    {
        // If using Hold Trigger in Input Action, Triggered is enough.
        // Otherwise use elapsed time.
        return (TriggerEvent == ETriggerEvent::Ongoing || TriggerEvent == ETriggerEvent::Triggered) && InputValue.GetElapsedTime() >= HoldTimeThreshold;
    }
    case EInteractionInputMode::ChargedRelease:
    {
        // If using Hold Trigger in Input Action, Triggered is enough.
        // Otherwise use elapsed time.
        return TriggerEvent == ETriggerEvent::Triggered && InputValue.GetElapsedTime() >= HoldTimeThreshold;
    }
    case EInteractionInputMode::DoubleClick:
    {
        const UWorld *World = GetWorld();
        if (!World || TriggerEvent != ETriggerEvent::Started)
        {
            return false;
        }

        const float CurrentTime = World->GetTimeSeconds();
        const bool bIsDoubleClick = (CurrentTime - LastPressTime) <= DoubleTapInterval;
        LastPressTime = CurrentTime;
        return bIsDoubleClick;
    }
    case EInteractionInputMode::Any:
    {
        return TriggerEvent == ETriggerEvent::Started || TriggerEvent == ETriggerEvent::Triggered || TriggerEvent == ETriggerEvent::Completed || TriggerEvent == ETriggerEvent::Ongoing || TriggerEvent == ETriggerEvent::Canceled;
    }
    default:
    {
        return false;
    }
    }
}

bool UInteractableComponent::CanInteract() const
{
    const UWorld *World = GetWorld();
    if (!World)
    {
        return false;
    }

    switch (InteractMode)
    {
    case EInteractionUsageMode::Once:
    {
        return !bHasInteracted;
    }

    case EInteractionUsageMode::Multi:
    {
        return true;
    }

    case EInteractionUsageMode::Cooldown:
    {
        return World->GetTimeSeconds() >= NextAllowedInteractTime;
    }

    default:
    {
        return false;
    }
    }
}
