// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSInteraction.h"
#include "DrawDebugHelpers.h"

void UFPSInteraction::OnControllerReady(AController *InController)
{
    if (InController == nullptr)
        return;

    APlayerController *PC = Cast<APlayerController>(InController);
    if (PC == nullptr)
    {
        PC = GetWorld()->GetFirstPlayerController();
    }
    SetPivotToComponent(PC->PlayerCameraManager->GetTransformComponent());
}

bool UFPSInteraction::TryGetDetectedFocused(FHitResult &OutHit) const
{
    UWorld *World = GetWorld();
    if (!World)
    {
        return false;
    }

    const FTransform Pivot = GetPivot();

    const FVector Start = Pivot.GetLocation() + Offcet;
    const FVector Direction = GetTraceDirection(Pivot);
    const FVector End = Start + Direction * TraceDistance;

    FCollisionQueryParams QueryParams;
    QueryParams.bTraceComplex = false;
    QueryParams.AddIgnoredActor(GetOwner());

    const bool bHit = World->LineTraceSingleByChannel(
        OutHit,
        Start,
        End,
        TraceChannel,
        QueryParams);

#if ENABLE_DRAW_DEBUG
    if (DebugDrawType != EDebugDrawType::None)
    {
        const FColor LineColor = bHit ? DebugHitColor : DebugNoHitColor;

        switch (DebugDrawType)
        {
        case EDebugDrawType::Line:
            DrawDebugLine(World, Start, End, LineColor, false, 1.f, 0, 1.f);
            if (bHit)
            {
                DrawDebugPoint(World, OutHit.ImpactPoint, 10.f, LineColor, false, 1.f);
            }
            break;

        case EDebugDrawType::Sphere:
            if (bHit)
            {
                DrawDebugSphere(World, OutHit.ImpactPoint, 12.f, 16, LineColor, false, 1.f, 0, 1.5f);
            }
            break;

        case EDebugDrawType::LineAndSphere:
            DrawDebugLine(World, Start, End, LineColor, false, 1.f, 0, 1.f);
            if (bHit)
            {
                DrawDebugSphere(World, OutHit.ImpactPoint, 12.f, 16, LineColor, false, 1.f, 0, 1.5f);
                DrawDebugPoint(World, OutHit.ImpactPoint, 10.f, LineColor, false, 1.f);
            }
            break;
        }
    }
#endif

    return bHit;
}

// Functions
FVector UFPSInteraction::GetTraceDirection(const FTransform &Pivot) const
{
    switch (TraceAxis)
    {
    case EInteractionTraceAxis::Right:
        return Pivot.GetUnitAxis(EAxis::Y);

    case EInteractionTraceAxis::Up:
        return Pivot.GetUnitAxis(EAxis::Z);

    case EInteractionTraceAxis::Forward:
    default:
        return Pivot.GetUnitAxis(EAxis::X);
    }
}
