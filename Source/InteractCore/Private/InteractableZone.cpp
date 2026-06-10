// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractableZone.h"
#if WITH_EDITOR
#include "DrawDebugHelpers.h"
#endif

UInteractableZone::UInteractableZone()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickInterval = 0.04f; // 1 / 25 = 0.04 seconds
    bIsImplememtSetupZone = GetClass()->IsFunctionImplementedInScript(GET_FUNCTION_NAME_CHECKED(UInteractableZone, K2_SetupZone));
    PRINT("Child");
}

void UInteractableZone::OnRegister()
{
    Super::OnRegister();

#if WITH_EDITOR
    if (!GetVisibleFlag())
        return;

    UWorld *World = GetWorld();
    if (!World)
        return;

    const FVector SphereWorldLocation = GetComponentTransform().TransformPosition(ZoneOffset);

    DrawDebugSphere(
        World,
        SphereWorldLocation,
        ZoneRadius,
        15,
        FColor::Black,
        false,
        0.05f,
        0,
        1);
#endif
}
void UInteractableZone::BeginPlay()
{
    Super::BeginPlay();

    UWorld *World = GetWorld();
    if (!SphereZone && World)
    {
        SphereZone = NewObject<USphereComponent>(GetOwner(), MakeUniqueObjectName(GetOwner(), USphereComponent::StaticClass(), TEXT("DetectionSphere")));
        SphereZone->SetupAttachment(this);
        SphereZone->RegisterComponent();

        SetupZone(SphereZone);
        if (bIsImplememtSetupZone)
            K2_SetupZone(SphereZone);
    }

    if (SphereZone != nullptr)
    {
        SphereZone->OnComponentBeginOverlap.AddDynamic(this, &UInteractableZone::OnOverlapBegin);
        SphereZone->OnComponentEndOverlap.AddDynamic(this, &UInteractableZone::OnOverlapEnd);
    }
}

void UInteractableZone::OnOverlapBegin(UPrimitiveComponent *OverlappedComp, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
    if (OtherActor && OtherActor != GetOwner())
    {
        UE_LOG(LogTemp, Warning, TEXT("%s Began Overlap with: %s"), *GetName(), *OtherActor->GetName());
        // Your Custom Begin Overlap Logic Here
    }
}
void UInteractableZone::OnOverlapEnd(UPrimitiveComponent *OverlappedComp, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex)
{
    if (OtherActor && OtherActor != GetOwner())
    {
        UE_LOG(LogTemp, Warning, TEXT("%s Ended Overlap with: %s"), *GetName(), *OtherActor->GetName());
        // Your Custom End Overlap Logic Here
    }
}

void UInteractableZone::SetupZone(USphereComponent *Zone) const
{
    Zone->SetRelativeLocation(ZoneOffset);
    // SphereComp->SetRelativeRotation(FRotator::ZeroRotator);
    // SphereComp->SetRelativeScale3D(FVector(1.f));
    Zone->SetSphereRadius(ZoneRadius);
    Zone->SetGenerateOverlapEvents(true);
    Zone->SetCollisionEnabled(ZoneCollision);
    Zone->SetCollisionObjectType(ZoneCollisionObjectType);
    if (ZoneCollisionProfile.Name != NAME_None)
    {
        Zone->SetCollisionProfileName(ZoneCollisionProfile.Name);
    }
    Zone->bHiddenInGame = bHiddenInGame;
    Zone->SetVisibleFlag(GetVisibleFlag());
}

void UInteractableZone::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    // Unbind overlap events
    if (SphereZone != nullptr)
    {
        SphereZone->OnComponentBeginOverlap.RemoveDynamic(this, &UInteractableZone::OnOverlapBegin);
        SphereZone->OnComponentEndOverlap.RemoveDynamic(this, &UInteractableZone::OnOverlapEnd);
    }

    Super::EndPlay(EndPlayReason);
}

#if WITH_EDITOR
void UInteractableZone::ShowDebug()
{
    UWorld *World = GetWorld();
    if (!World)
        return;

    const FVector SphereWorldLocation = GetComponentTransform().TransformPosition(ZoneOffset);
    const FColor ZoneColor = FColor(223, 149, 157, 255);

    DrawDebugSphere(
        World,
        SphereWorldLocation,
        ZoneRadius,
        15,
        ZoneColor,
        false,
        5,
        0,
        1);
}
#endif
