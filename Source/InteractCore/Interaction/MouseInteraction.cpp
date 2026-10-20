// Fill out your copyright notice in the Description page of Project Settings.

#include "MouseInteraction.h"

void UMouseInteraction::OnControllerReady(AController *InController)
{
    if (InController == nullptr)
        return;

    PC = Cast<APlayerController>(InController);
}

bool UMouseInteraction::TryGetDetectedFocused(FHitResult &OutHit) const
{
    OutHit = FHitResult();

    UWorld *World = GetWorld();
    if (!World)
    {
        LOG_WARNING("World is not valid");
        return false;
    }
    if (!PC)
    {
        LOG_WARNING("PlayerController is not valid");
        return false;
    }

    float MouseX = 0.f;
    float MouseY = 0.f;
    if (!PC->GetMousePosition(MouseX, MouseY))
    {
        return false;
    }

    FVector WorldLocation;
    FVector WorldDirection;
    if (!PC->DeprojectScreenPositionToWorld(MouseX, MouseY, WorldLocation, WorldDirection))
    {
        return false;
    }

    const FVector TraceStart = WorldLocation;
    const FVector TraceEnd = TraceStart + (WorldDirection * TraceDistance);

    const bool bHit = World->LineTraceSingleByChannel(
        OutHit,
        TraceStart,
        TraceEnd,
        TraceChannel,
        BuildQueryParams());

    if (bDrawDebug)
    {
        DrawDebugLine(
            World,
            TraceStart,
            TraceEnd,
            bHit ? FColor::Red : DebugColor,
            false,
            1.f,
            0,
            1.5f);

        if (bHit)
        {
            DrawDebugSphere(
                World,
                OutHit.ImpactPoint,
                8.f,
                12,
                FColor::Yellow,
                false,
                1.f);
        }
    }

    return bHit;
}

FCollisionQueryParams UMouseInteraction::BuildQueryParams() const
{
    FCollisionQueryParams Params(SCENE_QUERY_STAT(MouseTrace), bTraceComplex);
    Params.AddIgnoredActors(GetIgnoredActors()); // calls the BP override if there is one
    return Params;
}
