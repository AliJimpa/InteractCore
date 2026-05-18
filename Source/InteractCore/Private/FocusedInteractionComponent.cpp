// Fill out your copyright notice in the Description page of Project Settings.

#include "FocusedInteractionComponent.h"

bool UFocusedInteractionComponent::TryGetDetectedFocused(AController *Controller, FHitResult &OutHit) const
{
    FVector Start;
    FVector End;

    APlayerController *PC = Cast<APlayerController>(Controller);
    if (!PC)
    {
        LOG_WARNING("Pawn has no PlayerController yet");
        return false;
    }

    FVector CameraLocation;
    FRotator CameraRotation;

    PC->GetPlayerViewPoint(CameraLocation, CameraRotation);

    Start = CameraLocation;
    End = Start + (CameraRotation.Vector() * 500.f); // 500 = trace distance

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(GetOwner());

    bool bHit = GetWorld()->LineTraceSingleByChannel(
        OutHit,
        Start,
        End,
        ECC_Visibility,
        Params);

    if (bHit)
    {
        UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *OutHit.GetActor()->GetName());
        DrawDebugPoint(
            GetWorld(),
            OutHit.Location,
            10.f,
            FColor::Red,
            false,
            1.0f);
    }
    else
    {
        DrawDebugLine(
            GetWorld(),
            Start,
            End,
            FColor::Green,
            false,
            1.0f,
            0,
            1.0f);
    }

    return bHit;
}