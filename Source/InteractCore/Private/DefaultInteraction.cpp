// Fill out your copyright notice in the Description page of Project Settings.

#include "DefaultInteraction.h"

UDefaultInteraction::UDefaultInteraction() : Super()
{
    bIsImplementCustomAdaptiveTick = GetClass()->IsFunctionImplementedInScript(GET_FUNCTION_NAME_CHECKED(UDefaultInteraction, K2_CustomAdaptiveTick));
    bIsImplememtCanHover = GetClass()->IsFunctionImplementedInScript(GET_FUNCTION_NAME_CHECKED(UDefaultInteraction, K2_CanHover));
    bIsImplememtCanInteract = GetClass()->IsFunctionImplementedInScript(GET_FUNCTION_NAME_CHECKED(UDefaultInteraction, K2_CanInteract));
}

void UDefaultInteraction::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
    if (bDynamicInterval)
    {
        UpdateAdaptiveTickRate();
    }

    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

// Permissions
bool UDefaultInteraction::CanHover(UObject *Interactable) const
{
    if (bIsImplememtCanHover)
        return K2_CanHover(Interactable);
    return true;
}
bool UDefaultInteraction::CanInteract(UObject *Interactable) const
{
    if (bIsImplememtCanInteract)
        return K2_CanInteract(Interactable);
    return true;
}

// TickInterval
void UDefaultInteraction::UpdateAdaptiveTickRate()
{
    // 1. Get current states
    const FTransform CurrentPivot = GetPivot();

    bool bTriggerFastTick = false;

    // 2. Evaluate Trigger logic based on Enum
    switch (AdaptiveTickMode)
    {
    case EInteractionAdaptiveTickTrigger::PivotRotation:
    {
        float AngleDelta = CurrentPivot.GetRotation().AngularDistance(PreviousPivotTransform.GetRotation());
        bTriggerFastTick = FMath::RadiansToDegrees(AngleDelta) > RotationThreshold;
    }
    break;

    case EInteractionAdaptiveTickTrigger::PivotPosition:
    {
        float DistDelta = FVector::Dist(CurrentPivot.GetLocation(), PreviousPivotTransform.GetLocation());
        bTriggerFastTick = DistDelta > PositionThreshold;
    }
    break;

    case EInteractionAdaptiveTickTrigger::PivotTransform:
    {
        float AngleDelta = CurrentPivot.GetRotation().AngularDistance(PreviousPivotTransform.GetRotation());
        float DistDelta = FVector::Dist(CurrentPivot.GetLocation(), PreviousPivotTransform.GetLocation());
        bTriggerFastTick = (AngleDelta > RotationThreshold) || (DistDelta > PositionThreshold);
    }
    break;

    case EInteractionAdaptiveTickTrigger::OwnerVelocity:
    {
        AActor *Owner = GetOwner();
        if (Owner)
        {
            // Check if actor is moving faster than the threshold
            bTriggerFastTick = Owner->GetVelocity().Size() > VelocityThreshold;
        }
    }
    break;

    case EInteractionAdaptiveTickTrigger::MouseMovement:
    {
        APlayerController *PC = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr;
        if (PC)
        {
            float MouseX, MouseY;
            if (PC->GetMousePosition(MouseX, MouseY))
            {
                FVector2D CurrentMousePos(MouseX, MouseY);
                bTriggerFastTick = FVector2D::Distance(CurrentMousePos, PreviousMousePosition) > MouseMovementThreshold;
                PreviousMousePosition = CurrentMousePos; // Update mouse state
            }
        }
    }
    break;

    default:
        if (bIsImplementCustomAdaptiveTick)
        {
            bTriggerFastTick = K2_CustomAdaptiveTick(ThresholdCustom);
        }
        else
        {
            bTriggerFastTick = CustomAdaptiveTick(ThresholdCustom);
        }
        break;
    }

    float TargetInterval = bTriggerFastTick ? FastTickRate : SlowTickRate;
    if (!FMath::IsNearlyEqual(CurrentInterval, TargetInterval))
    {
        CurrentInterval = TargetInterval;
        SetComponentTickInterval(CurrentInterval);
    }
    PreviousPivotTransform = CurrentPivot;
}