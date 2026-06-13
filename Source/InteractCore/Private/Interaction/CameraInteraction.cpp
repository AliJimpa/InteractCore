// Fill out your copyright notice in the Description page of Project Settings.

#include "CameraInteraction.h"
#include "DrawDebugHelpers.h"

UCameraInteraction::UCameraInteraction() : Super()
{
    if (TraceType == EInteractionTraceType::Auto && SphereTraceRadii.IsEmpty())
    {
        SphereTraceRadii = {5.f, 10.f, 15.f};
    }
}

void UCameraInteraction::OnControllerReady(AController *InController)
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

bool UCameraInteraction::TryGetDetectedFocused(FHitResult &OutHit) const
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

    bool bHit = false;
    if (TraceType == EInteractionTraceType::LineTrace || TraceType == EInteractionTraceType::Auto)
    {
        bHit = World->LineTraceSingleByChannel(
            OutHit,
            Start,
            End,
            TraceChannel,
            QueryParams);
#if ENABLE_DRAW_DEBUG
        if (bDrawDebugTrace)
        {
            const FColor LineColor = bHit ? DebugHitColor : DebugNoHitColor;
            DrawDebugLine(World, Start, End, LineColor, false, 1.f, 0, 1.f);
            if (bHit)
            {
                DrawDebugPoint(World, OutHit.ImpactPoint, 10.f, LineColor, false, 1.5f);
            }
        }
#endif
    }

    if (TraceType == EInteractionTraceType::SphereTrace || (TraceType == EInteractionTraceType::Auto && !bHit))
    {
        // Sphere Fallback
        for (float Radius : SphereTraceRadii)
        {
            bHit = World->SweepSingleByChannel(
                OutHit,
                Start,
                End,
                FQuat::Identity,
                TraceChannel,
                FCollisionShape::MakeSphere(Radius),
                QueryParams);
#if ENABLE_DRAW_DEBUG
            if (bDrawDebugTrace)
            {
                const FColor LineColor = bHit ? DebugHitColor : DebugNoHitColor;
                DrawDebugSphere(World, End, Radius, 16, LineColor, false, 1.f, 0, 1.5f);
                if (bHit)
                {
                    DrawDebugPoint(World, OutHit.ImpactPoint, 10.f, LineColor, false, 1.5f);
                }
            }
#endif
            if(bHit)
                break;
        }
    }

    return bHit;
}

// Functions
FVector UCameraInteraction::GetTraceDirection(const FTransform &Pivot) const
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
