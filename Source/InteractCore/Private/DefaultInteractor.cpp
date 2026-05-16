// Fill out your copyright notice in the Description page of Project Settings.

#include "DefaultInteractor.h"

bool UDefaultInteractor::PerformSingleTrace(FInteractionData &OutData)
{
    FASTPRINT("C++");
    AActor *Owner = GetOwner();
    if (!Owner)
        return false;

    FVector Start = Owner->GetActorLocation();
    FVector Forward = Owner->GetActorForwardVector();
    FVector End = Start + Forward * 500.f;

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(Owner);

    bool bHit = GetWorld()->LineTraceSingleByChannel(
        Hit,
        Start,
        End,
        ECC_Visibility,
        Params);

    if (bHit && Hit.bBlockingHit)
    {
        OutData.SetHit(0, Hit);
        return true;
    }

    return false;
}

bool UDefaultInteractor::PerformMultiTrace(FInteractionData &OutData)
{
    AActor *Owner = GetOwner();
    if (!Owner)
        return false;

    FVector Start = Owner->GetActorLocation();
    FVector End = Start + Owner->GetActorForwardVector() * 500.f;

    TArray<FHitResult> Hits;

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(Owner);

    bool bHit = GetWorld()->SweepMultiByChannel(
        Hits,
        Start,
        End,
        FQuat::Identity,
        ECC_Visibility,
        FCollisionShape::MakeSphere(50.f),
        Params);

    if (!bHit)
        return false;

    int32 Count = FMath::Min(Hits.Num(), MAXHITS);

    for (int32 i = 0; i < Count; ++i)
    {
        if (Hits[i].bBlockingHit)
        {
            OutData.SetHit(i, Hits[i]);
        }
    }

    return OutData.HasAnyRecord();
}

const FInteractionRecord *UDefaultInteractor::SelectInteractionTarget(const FInteractionData &HoverData)
{
    return nullptr;
}