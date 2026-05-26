// Fill out your copyright notice in the Description page of Project Settings.

#include "ProximityInteraction.h"
#include "Components/SphereComponent.h"
#include "EnhancedPlayerInput.h"

UProximityInteraction::UProximityInteraction()
{
    DetectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DetectionSphere"));
}

// Basic Methods
void UProximityInteraction::BeginPlay()
{
    DetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &UProximityInteraction::OnBeginOverlap);
    DetectionSphere->OnComponentEndOverlap.AddDynamic(this, &UProximityInteraction::OnEndOverlap);
    Super::BeginPlay();
}
void UProximityInteraction::OnRegister()
{
    Super::OnRegister();

    if (!DetectionSphere)
        return;

    AActor *Owner = GetOwner();
    if (!Owner)
        return;

    if (USceneComponent *Root = Owner->GetRootComponent())
    {
        DetectionSphere->AttachToComponent(Root, FAttachmentTransformRules::KeepRelativeTransform);
    }

    DetectionSphere->RegisterComponent();
}
void UProximityInteraction::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
    if (UseInputForHovering)
    {
        if (!IsHoverInputPressed())
        {
            return;
        }
    }
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
void UProximityInteraction::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    DetectionSphere->OnComponentBeginOverlap.RemoveDynamic(this, &UProximityInteraction::OnBeginOverlap);
    DetectionSphere->OnComponentEndOverlap.RemoveDynamic(this, &UProximityInteraction::OnEndOverlap);

    Super::EndPlay(EndPlayReason);
}

// Interaction Method
void UProximityInteraction::OnControllerReady(AController *InController)
{
    // Super::OnControllerReady(InController);

    if (UseInputForHovering)
    {
        if (InController == nullptr)
        {
            LOG_ERROR("Controller not Ready");
            return;
        }

        if (InController->IsLocalController())
        {
            APlayerController *PC = Cast<APlayerController>(InController);
            if (!PC)
            {
                LOG_ERROR("Pawn has no PlayerController yet");
                return;
            }

            EPI = Cast<UEnhancedPlayerInput>(PC->PlayerInput);
            if (EPI == nullptr)
            {
                LOG_WARNING("APlayerController has not any EnhancePlayerInput");
                PC = GetWorld()->GetFirstPlayerController();
            }
        }
        else
        {
            LOG_ERROR("The Owner Actor [%s] is not Ownership to get controller", *GetOwner()->GetName());
            return;
        }
    }

    SetPivotToComponent(GetOwner()->GetRootComponent());
}
bool UProximityInteraction::TryGetDetectedFocused(FHitResult &OutHit) const
{
    if (CandidateHits.Num() == 0)
    {
        return false;
    }

    const AActor *Owner = GetOwner();
    if (!Owner)
    {
        return false;
    }

    const FVector Pivot = GetPivot().GetLocation();

    float ClosestDistSq = TNumericLimits<float>::Max();
    const FHitResult *BestHit = nullptr;

    for (const FHitResult &Hit : CandidateHits)
    {
        AActor *HitActor = Hit.GetActor();

        if (!IsValid(HitActor))
        {
            continue;
        }
        const FVector TargetLocation = Hit.ImpactPoint;

        const float DistSq = FVector::DistSquared(Pivot, TargetLocation);

        if (DistSq < ClosestDistSq)
        {
            ClosestDistSq = DistSq;
            BestHit = &Hit;
        }
    }

    if (BestHit)
    {
        OutHit = *BestHit;
        return true;
    }

    return false;
}

// Collition Methods
void UProximityInteraction::OnBeginOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
    if (!OtherActor || OtherActor == GetOwner())
    {
        return;
    }

    // Avoid duplicates for same actor/component
    const int32 ExistingIndex = CandidateHits.IndexOfByPredicate(
        [OtherActor, OtherComp](const FHitResult &Hit)
        {
            return Hit.GetActor() == OtherActor && Hit.GetComponent() == OtherComp;
        });

    if (ExistingIndex != INDEX_NONE)
    {
        CandidateHits[ExistingIndex] = SweepResult;
    }
    else
    {
        CandidateHits.Add(SweepResult);
        LOG("NEWItemAdded");
    }
}
void UProximityInteraction::OnEndOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex)
{
    if (!OtherActor)
    {
        return;
    }

    CandidateHits.RemoveAll(
        [OtherActor, OtherComp](const FHitResult &Hit)
        {
            return Hit.GetActor() == OtherActor && Hit.GetComponent() == OtherComp;
        });
}

// Input Methods
bool UProximityInteraction::IsHoverInputPressed() const
{
    if (!HoverInput)
    {
        LOG_ERROR("HoverInput is not valid!");
        return false;
    }

    if (!EPI)
    {
        return false;
    }

    return EPI->GetActionValue(HoverInput).Get<bool>();
}

#if WITH_EDITOR
void UProximityInteraction::PostEditChangeProperty(FPropertyChangedEvent &PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    DetectionSphere->SetWorldLocation(Offcet);
}
#endif
