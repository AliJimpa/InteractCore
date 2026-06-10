// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractableZone.h"
#if WITH_EDITOR
#include "DrawDebugHelpers.h"
#endif

UInteractableZone::UInteractableZone()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickInterval = 0.04f; // 1 / 25 = 0.04 seconds
    bIsImplememtZoneSettings = GetClass()->IsFunctionImplementedInScript(GET_FUNCTION_NAME_CHECKED(UInteractableZone, K2_ApplyZoneSettings));
    RadiusRange = FVector2D(SphereRadius, SphereRadius * 2);
    // PRINT("Child");
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

        ApplyZoneSettings(SphereZone);
        if (bIsImplememtZoneSettings)
            K2_ApplyZoneSettings(SphereZone);
    }

    if (SphereZone != nullptr)
    {
        SphereZone->OnComponentBeginOverlap.AddDynamic(this, &UInteractableZone::OnOverlapBegin);
        SphereZone->OnComponentEndOverlap.AddDynamic(this, &UInteractableZone::OnOverlapEnd);
    }

    TArray<FHitResult> Hits;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(GetOwner());
    GetWorld()->SweepMultiByChannel(Hits, GetOwner()->GetActorLocation(), GetOwner()->GetActorLocation(), FQuat::Identity, SightChannel, FCollisionShape::MakeSphere(ZoneRadius), Params);
    for (const FHitResult &Hit : Hits)
    {
        AActor *HitActor = Hit.GetActor();
        if (!HitActor)
            continue;

        UInteractionComponent *Comp = HitActor->FindComponentByClass<UInteractionComponent>();
        if (Comp && !HasDetectedTarget())
        {
            DetectedObj = Comp;
            OnInteractorDetected(Comp);
            OnZoneBegin.Broadcast(Comp);
        }
    }
}

void UInteractableZone::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (bCheckLineOfSight)
    {
        if (IsValid(DetectedObj))
        {
            bCanSee = CheckLineOfSight();
        }
        else
        {
            bCanSee = false;
        }
    }

    if (bDynamicRadius && HasDetectedTarget())
    {
        // Distance from character to the InteractablePoint sphere
        const FVector SelfPos = GetComponentLocation();
        const FVector TargetPos = DetectedObj->GetOwner()->GetActorLocation();
        const float Distance = FVector::Dist(TargetPos, SelfPos);
        const float NormalizedDistance = FMath::Clamp(DistanceThreshold / Distance, 0.0f, 1.0f);
        const float CurrentRadius = FMath::Lerp(RadiusRange.X, RadiusRange.Y, NormalizedDistance);
        SetSphereRadius(CurrentRadius);
    }
}

bool UInteractableZone::CheckLineOfSight() const
{
    // Start from this component location
    const FVector Start = GetComponentTransform().GetLocation();
    // Aim for the target to reduce false negatives
    const FVector End = DetectedObj->GetOwner()->GetActorLocation();

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(GetOwner()); // ignore self only

    bool bSeen = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, SightChannel, Params);

    // Seen only if the trace hit the target actor directly
    if (bSeen)
        bSeen = Hit.GetActor() == DetectedObj->GetOwner();

#if WITH_EDITOR
    if (bShowDebugSight)
        DrawDebugLine(GetWorld(), Start, End, Hit.bBlockingHit ? FColor::Green : FColor::Red, false, 0.f, 0, 1.f);
#endif

    return bSeen;
}

void UInteractableZone::OnOverlapBegin(UPrimitiveComponent *OverlappedComp, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
    if (OtherActor && OtherActor != GetOwner())
    {
        // UE_LOG(LogTemp, Warning, TEXT("%s Began Overlap with: %s"), *GetName(), *OtherActor->GetName());
        UInteractionComponent *Comp = OtherActor->FindComponentByClass<UInteractionComponent>();
        if (Comp && !HasDetectedTarget())
        {
            DetectedObj = Comp;
            OnInteractorDetected(Comp);
            OnZoneBegin.Broadcast(Comp);
        }
    }
}
void UInteractableZone::OnOverlapEnd(UPrimitiveComponent *OverlappedComp, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex)
{
    if (OtherActor && OtherActor != GetOwner())
    {
        // UE_LOG(LogTemp, Warning, TEXT("%s Ended Overlap with: %s"), *GetName(), *OtherActor->GetName());
        UInteractionComponent *Comp = OtherActor->FindComponentByClass<UInteractionComponent>();
        if (Comp && HasDetectedTarget())
        {
            DetectedObj = nullptr;
            OnInteractorLost(Comp);
            OnZoneEnd.Broadcast(Comp);
        }
    }
}

void UInteractableZone::ApplyZoneSettings(USphereComponent *Zone) const
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

void UInteractableZone::OnInteractorDetected(UInteractionComponent *Interactor)
{
    if (bToggleRadius)
        SetSphereRadius(ToggleRadius);
}
void UInteractableZone::OnInteractorLost(UInteractionComponent *Interactor)
{
    if (bToggleRadius)
        SetSphereRadius(ZoneRadius);
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
