// Fill out your copyright notice in the Description page of Project Settings.

#include "MouseInteraction.h"

void UMouseInteraction::OnControllerReady(AController *InController)
{
    if (InController == nullptr)
        return;

    PC = Cast<APlayerController>(InController);
    if (PC == nullptr)
    {
        PC = GetWorld()->GetFirstPlayerController();
    }
}

bool UMouseInteraction::TryGetDetectedFocused(FHitResult &OutHit) const
{
    OutHit = FHitResult();

    UWorld *World = GetWorld();
    if (!World)
    {
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

    FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(MouseTrace), bTraceComplex);
    QueryParams.AddIgnoredActor(GetOwner());

    const bool bHit = World->LineTraceSingleByChannel(
        OutHit,
        TraceStart,
        TraceEnd,
        TraceChannel,
        QueryParams);

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
