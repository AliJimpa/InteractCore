// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractableComponent.h"
#include "EnhancedInput\Public\InputTriggers.h"

UInteractableComponent::UInteractableComponent()
{
    SphereRadius = 32.0f;
    SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    SetCollisionResponseToAllChannels(ECR_Ignore);
    SetCollisionResponseToChannel(InteractChannel, ECR_Block);
    //RINT("Base");
}

void UInteractableComponent::OnRegister()
{
    Super::OnRegister();

    SetCollisionResponseToChannel(InteractChannel, ECR_Block);
}

void UInteractableComponent::Interact_Implementation(UInteractionComponent *Provider)
{
    OnInteract.Broadcast(Provider);
    LOG("INTERACT");
}
void UInteractableComponent::Hover_Implementation(UInteractionComponent *Provider, FHitResult Hit)
{
    bIsHovered = true;
    OnHoverBegin.Broadcast(Provider);
    LOG("HOVER");
}
void UInteractableComponent::UnHover_Implementation(UInteractionComponent *Provider)
{
    bIsHovered = false;
    OnHoverEnd.Broadcast(Provider);
    LOG("UNHOVER");
}