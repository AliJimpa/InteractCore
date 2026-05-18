// Fill out your copyright notice in the Description page of Project Settings.

#include "AreaInteractionComponent.h"
#include "Components/SphereComponent.h"
// #include "EnhancedInputComponent.h"
// #include "GameFramework/PlayerController.h"
#include "EnhancedPlayerInput.h"

UAreaInteractionComponent::UAreaInteractionComponent()
{
    DetectionSphere = CreateDefaultSubobject<USphereComponent>("InteractionSphere");
    DetectionSphere->SetSphereRadius(300.f);
}

void UAreaInteractionComponent::OnControllerReady(AController *InController)
{
    Super::OnControllerReady(InController);

    if (InController == nullptr)
        return;

    if (InController->IsLocalController())
    {
        APlayerController *PC = Cast<APlayerController>(InController);
        if (!PC)
        {
            LOG_WARNING("Pawn has no PlayerController yet");
            return;
        }

        EPI = Cast<UEnhancedPlayerInput>(PC->PlayerInput);
        if (!EPI)
        {
            LOG_WARNING("APlayerController has not any EnhancePlayerInput");
            return;
        }
        else
        {
            LOG("TEST");
            return;
        }
    }
    else
    {
        LOG_ERROR("The Owner Actor [%s] is not Ownership to get controller", *GetOwner()->GetName());
        return;
    }
}

void UAreaInteractionComponent::BeginPlay2()
{
    Super::BeginPlay();

    DetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &UAreaInteractionComponent::OnBeginOverlap);
    DetectionSphere->OnComponentEndOverlap.AddDynamic(this, &UAreaInteractionComponent::OnEndOverlap);
}

void UAreaInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
    if (UseInputForHovering)
    {
        IsHoverPressed = IsHoverInputPressed();
        if (IsHoverPressed == false)
        {
            return;
        }
    }
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UAreaInteractionComponent::DetectCandidates()
{
    // candidates already tracked by overlap events
}

void UAreaInteractionComponent::OnBeginOverlap(
    UPrimitiveComponent *OverlappedComponent,
    AActor *OtherActor,
    UPrimitiveComponent *OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult &SweepResult)
{
    CandidateActors.AddUnique(OtherActor);
}

void UAreaInteractionComponent::OnEndOverlap(
    UPrimitiveComponent *OverlappedComponent,
    AActor *OtherActor,
    UPrimitiveComponent *OtherComp,
    int32 OtherBodyIndex)
{
    CandidateActors.Remove(OtherActor);
}

bool UAreaInteractionComponent::CanHover(UObject *Interactable) const
{
    Super::CanHover(Interactable);
    if (UseInputForHovering)
        return IsHoverPressed;

    return true;
}

bool UAreaInteractionComponent::IsHoverInputPressed() const
{
    if (!HoverInput)
    {
        LOG_ERROR("HoverInput is not valid!");
        return false;
    }

    if (!EPI)
    {
        return false;
    }

    return EPI->GetActionValue(HoverInput).Get<bool>();
}