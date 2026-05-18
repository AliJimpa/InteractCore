// Fill out your copyright notice in the Description page of Project Settings.

#include "FPS_Interactor.h"
#include "DrawDebugHelpers.h"

void UFPS_Interactor::OnControllerReady(AController *InController)
{
    Super::OnControllerReady(InController);

    APlayerController *PC = Cast<APlayerController>(InController);
    if (PC != nullptr)
    {
        // APlayerCameraManager *CameraManager = PC->PlayerCameraManager;
        FVector CameraLocation;
        FRotator CameraRotation;
        PC->GetPlayerViewPoint(CameraLocation, CameraRotation);
        FTransform CameraTransform;
        CameraTransform.SetLocation(CameraLocation);
        CameraTransform.SetRotation(CameraRotation.Quaternion());
        CameraTransform.SetScale3D(FVector(1.f));
        SetPivotToTransform(CameraTransform);
    }
}

bool UFPS_Interactor::TryGetDetectedFocused(FHitResult &OutHit) const
{
    UWorld *World = GetWorld();
    if (!World)
    {
        return false;
    }

    const FTransform Pivot = GetPivot();

    const FVector Start = Pivot.GetLocation();
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
    if (bDrawDebugTrace)
    {
        const FColor LineColor = bHit ? DebugHitColor : DebugNoHitColor;
        DrawDebugLine(World, Start, End, LineColor, false, 1.f, 0, 1.f);

        if (bHit)
        {
            DrawDebugPoint(World, OutHit.ImpactPoint, 10.f, LineColor, false, 1.f);
        }
    }
#endif

    return bHit;
}

// Functions
FVector UFPS_Interactor::GetTraceDirection(const FTransform &Pivot) const
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
