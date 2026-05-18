// Fill out your copyright notice in the Description page of Project Settings.

#include "DefaultInteractor.h"

UDefaultInteractor::UDefaultInteractor() : Super()
{
    bIsImplementCustomAdaptiveTick = GetClass()->IsFunctionImplementedInScript(GET_FUNCTION_NAME_CHECKED(UDefaultInteractor, K2_CustomAdaptiveTick));
}

void UDefaultInteractor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
    if (bDynamicInterval)
    {
        float NewTickInterval;
        switch (AdaptiveTickMode)
        {
        case EInteractionAdaptiveTickMode::CameraRotation:
            UpdateTickByCameraRotation(ThresholdCameraRotation, NewTickInterval);
            break;

        case EInteractionAdaptiveTickMode::OwnerVelocity:
            UpdateTickByOwnerMovement(ThresholdOwnerVelocity, NewTickInterval);
            break;

        case EInteractionAdaptiveTickMode::MouseMovement:
            UpdateTickByMouse(ThresholdMouseMovement, NewTickInterval);
            break;

        default:
            if (bIsImplementCustomAdaptiveTick)
            {
                K2_CustomAdaptiveTick(ThresholdCustom, NewTickInterval);
            }
            else
            {
                CustomAdaptiveTick(ThresholdCustom, NewTickInterval);
            }
            break;
        }

        if (PrimaryComponentTick.TickInterval != NewTickInterval)
            PrimaryComponentTick.TickInterval = NewTickInterval;
    }

    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UDefaultInteractor::UpdateTickByCameraRotation(float Threshold, float &OutTickRate)
{
    OutTickRate = 0.033f;

    if (!CameraManager)
    {
        APlayerController *PC = GetWorld()->GetFirstPlayerController();
        if (!PC)
            return;

        CameraManager = PC->PlayerCameraManager;
        if (!CameraManager)
            return;
    }

    const FRotator CameraRot = CameraManager->GetCameraRotation();
    const float Delta = CameraRot.GetManhattanDistance(LastCameraRotation);

    OutTickRate = (Delta > Threshold) ? FastTickRate : SlowTickRate;

    LastCameraRotation = CameraRot;
}

void UDefaultInteractor::UpdateTickByOwnerMovement(float Threshold, float &OutTickRate)
{
    OutTickRate = SlowTickRate;

    AActor *Owner = GetOwner();
    if (!Owner)
        return;

    const float Speed = Owner->GetVelocity().Size();

    if (Speed > Threshold)
    {
        OutTickRate = FastTickRate;
    }
    else
    {
        OutTickRate = SlowTickRate;
    }

    return;
}

void UDefaultInteractor::UpdateTickByMouse(int32 Threshold, float &OutTickRate)
{
    OutTickRate = SlowTickRate;

    APlayerController *PC = GetWorld()->GetFirstPlayerController();
    if (!PC)
        return;

    float X, Y;
    if (!PC->GetMousePosition(X, Y))
        return;

    FVector2D Current(X, Y);

    const float Delta = FVector2D::Distance(Current, LastMousePosition);

    if (Delta > Threshold)
    {
        OutTickRate = FastTickRate;
    }
    else
    {
        OutTickRate = SlowTickRate;
    }

    LastMousePosition = Current;

    return;
}