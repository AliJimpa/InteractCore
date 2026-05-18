// Fill out your copyright notice in the Description page of Project Settings.

#include "FocusedInteractionComponent.h"

void UFocusedInteractionComponent::OnControllerReady(AController *InController)
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

bool UFocusedInteractionComponent::TryGetDetectedFocused(FHitResult &OutHit) const
{
    const FTransform CameraTransform = GetPivot();
    const FVector Start = CameraTransform.GetLocation();
    const FVector Forward = CameraTransform.GetUnitAxis(EAxis::X);
    const FVector End = Start + (Forward * 500.f); // 500 = trace distance

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