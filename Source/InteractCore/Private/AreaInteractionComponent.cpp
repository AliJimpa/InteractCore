// Fill out your copyright notice in the Description page of Project Settings.

#include "AreaInteractionComponent.h"
#include "Components/SphereComponent.h"

UAreaInteractionComponent::UAreaInteractionComponent()
{
    DetectionSphere = CreateDefaultSubobject<USphereComponent>("InteractionSphere");
    DetectionSphere->SetSphereRadius(300.f);
}

void UAreaInteractionComponent::BeginPlay2()
{
    Super::BeginPlay();

    DetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &UAreaInteractionComponent::OnBeginOverlap);
    DetectionSphere->OnComponentEndOverlap.AddDynamic(this, &UAreaInteractionComponent::OnEndOverlap);
}

void UAreaInteractionComponent::DetectCandidates()
{
    // candidates already tracked by overlap events
}

void UAreaInteractionComponent::OnBeginOverlap(
    UPrimitiveComponent *OverlappedComponent,
    AActor *OtherActor,
    UPrimitiveComponent *OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult &SweepResult)
{
    CandidateActors.AddUnique(OtherActor);
}

void UAreaInteractionComponent::OnEndOverlap(
    UPrimitiveComponent *OverlappedComponent,
    AActor *OtherActor,
    UPrimitiveComponent *OtherComp,
    int32 OtherBodyIndex)
{
    CandidateActors.Remove(OtherActor);
}