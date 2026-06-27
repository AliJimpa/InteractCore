// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractableDetection.h"
#include "Interaction/InteractionComponent.h"
#if WITH_EDITOR
#include "DrawDebugHelpers.h"
#endif

UInteractableDetection::UInteractableDetection()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickInterval = 0.04f; // 1 / 25 = 0.04 seconds
    bIsImplememtZoneSettings = GetClass()->IsFunctionImplementedInScript(GET_FUNCTION_NAME_CHECKED(UInteractableDetection, K2_ApplyZoneSettings));
    RadiusRange = FVector2D(SphereRadius, SphereRadius * 2);
    //PRINT("Sphere");
}

void UInteractableDetection::OnRegister()
{
    Super::OnRegister();

#if WITH_EDITOR
    if (!GetVisibleFlag())
        return;

    UWorld *World = GetWorld();
    if (!World)
        return;

    const FVector SphereWorldLocation = GetComponentTransform().TransformPosition(DetectionOffset);

    DrawDebugSphere(
        World,
        SphereWorldLocation,
        DetectionRadius,
        15,
        FColor::Black,
        false,
        0.05f,
        0,
        1);
#endif
}
void UInteractableDetection::BeginPlay()
{
    Super::BeginPlay();

    if (!DetectionComponent)
    {
        DetectionComponent = NewObject<USphereComponent>(GetOwner(), MakeUniqueObjectName(GetOwner(), USphereComponent::StaticClass(), TEXT("DetectionSphere")));
        DetectionComponent->SetupAttachment(this);
        DetectionComponent->RegisterComponent();
        ApplyZoneSettings(DetectionComponent);
        if (bIsImplememtZoneSettings)
            K2_ApplyZoneSettings(DetectionComponent);
    }

    if (DetectionComponent != nullptr)
    {
        DetectionComponent->OnComponentBeginOverlap.AddDynamic(this, &UInteractableDetection::OnOverlapBegin);
        DetectionComponent->OnComponentEndOverlap.AddDynamic(this, &UInteractableDetection::OnOverlapEnd);
    }

    if (bFirstCheck)
    {
        FirstCheck(DetectionRadius,DetectionChannel);
    }
}

void UInteractableDetection::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (bDynamicRadius && IsDetected())
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

void UInteractableDetection::FirstCheck_Implementation(float radius, ECollisionChannel Channel)
{
    TArray<FHitResult> Hits;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(GetOwner());
    GetWorld()->SweepMultiByChannel(Hits, GetOwner()->GetActorLocation(), GetOwner()->GetActorLocation(), FQuat::Identity, Channel, FCollisionShape::MakeSphere(radius), Params);
    for (const FHitResult &Hit : Hits)
    {
        AActor *HitActor = Hit.GetActor();
        if (!HitActor)
            continue;

        UInteractionComponent *Comp = HitActor->FindComponentByClass<UInteractionComponent>();
        if (Comp && !IsDetected())
        {
            DetectedObj = Comp;
            OnInteractorDetected(Comp);
            OnDetectionBegin.Broadcast(Comp);
        }
    }
}

void UInteractableDetection::OnOverlapBegin(UPrimitiveComponent *OverlappedComp, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
    if (OtherActor && OtherActor != GetOwner())
    {
        // UE_LOG(LogTemp, Warning, TEXT("%s Began Overlap with: %s"), *GetName(), *OtherActor->GetName());
        UInteractionComponent *Comp = OtherActor->FindComponentByClass<UInteractionComponent>();
        if (Comp && !IsDetected())
        {
            DetectedObj = Comp;
            OnInteractorDetected(Comp);
            OnDetectionBegin.Broadcast(Comp);
        }
    }
}
void UInteractableDetection::OnOverlapEnd(UPrimitiveComponent *OverlappedComp, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex)
{
    if (OtherActor && OtherActor != GetOwner())
    {
        // UE_LOG(LogTemp, Warning, TEXT("%s Ended Overlap with: %s"), *GetName(), *OtherActor->GetName());
        UInteractionComponent *Comp = OtherActor->FindComponentByClass<UInteractionComponent>();
        if (Comp && IsDetected())
        {
            DetectedObj = nullptr;
            OnInteractorLost(Comp);
            OnDetectionEnd.Broadcast(Comp);
        }
    }
}

void UInteractableDetection::ApplyZoneSettings(USphereComponent *Sphere) const
{
    Sphere->SetRelativeLocation(DetectionOffset);
    // SphereComp->SetRelativeRotation(FRotator::ZeroRotator);
    // SphereComp->SetRelativeScale3D(FVector(1.f));
    Sphere->SetSphereRadius(DetectionRadius);
    Sphere->SetGenerateOverlapEvents(true);
    Sphere->SetCollisionEnabled(DetectionCollision);
    Sphere->SetCollisionObjectType(DetectionObjectType);
    if (DetectionProfile.Name != NAME_None)
    {
        Sphere->SetCollisionProfileName(DetectionProfile.Name);
    }
    Sphere->bHiddenInGame = bHiddenInGame;
    Sphere->SetVisibleFlag(GetVisibleFlag());
}

void UInteractableDetection::OnInteractorDetected(UInteractionComponent *Interactor)
{
    if (bToggleRadiusbased)
        DetectionComponent->SetSphereRadius(ToggleRadius);
}
void UInteractableDetection::OnInteractorLost(UInteractionComponent *Interactor)
{
    if (bToggleRadiusbased)
        DetectionComponent->SetSphereRadius(DetectionRadius);
}

void UInteractableDetection::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    // Unbind overlap events
    if (DetectionComponent != nullptr)
    {
        DetectionComponent->OnComponentBeginOverlap.RemoveDynamic(this, &UInteractableDetection::OnOverlapBegin);
        DetectionComponent->OnComponentEndOverlap.RemoveDynamic(this, &UInteractableDetection::OnOverlapEnd);
        DetectionComponent->DestroyComponent();
        DetectionComponent = nullptr;
    }

    Super::EndPlay(EndPlayReason);
}

#if WITH_EDITOR
void UInteractableDetection::ShowDebug()
{
    UWorld *World = GetWorld();
    if (!World)
        return;

    const FVector SphereWorldLocation = GetComponentTransform().TransformPosition(DetectionOffset);
    const FColor ZoneColor = FColor(223, 149, 157, 255);

    DrawDebugSphere(
        World,
        SphereWorldLocation,
        DetectionRadius,
        15,
        ZoneColor,
        false,
        5,
        0,
        1);
}
#endif
