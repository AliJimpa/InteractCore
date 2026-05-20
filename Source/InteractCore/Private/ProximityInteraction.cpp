// Fill out your copyright notice in the Description page of Project Settings.

#include "ProximityInteraction.h"
#include "Components/SphereComponent.h"
// #include "EnhancedInputComponent.h"
// #include "GameFramework/PlayerController.h"
#include "EnhancedPlayerInput.h"

UProximityInteraction::UProximityInteraction()
{
    // Overlping just detect actor so hitresult didnt have any componet
    // if (DetectionMode == EInteractionSearchMode::ActorAndComponent)
    // {
    //     DetectionMode = EInteractionSearchMode::ActorOnly;
    // }

    DetectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
    DetectionSphere->InitSphereRadius(300.f);
    // Make it visible so you can see it in game/editor
    DetectionSphere->SetHiddenInGame(false);
    DetectionSphere->SetVisibility(true);
    // Not necessary for drawing, but helps debugging readability
    DetectionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    DetectionSphere->SetupAttachment(nullptr); // important: no parent yet in a component ctor
}

void UProximityInteraction::OnControllerReady(AController *InController)
{
    Super::OnControllerReady(InController);

    if (InController == nullptr)
        return;

    if (InController->IsLocalController())
    {
        APlayerController *PC = Cast<APlayerController>(InController);
        if (!PC)
        {
            LOG_WARNING("Pawn has no PlayerController yet");
            return;
        }

        EPI = Cast<UEnhancedPlayerInput>(PC->PlayerInput);
        if (!EPI)
        {
            LOG_WARNING("APlayerController has not any EnhancePlayerInput");
            return;
        }
    }
    else
    {
        LOG_ERROR("The Owner Actor [%s] is not Ownership to get controller", *GetOwner()->GetName());
        return;
    }
}

void UProximityInteraction::BeginPlay()
{
    Super::BeginPlay();
    if (AActor *Owner = GetOwner())
    {
        if (USceneComponent *Root = Owner->GetRootComponent())
        {
            DetectionSphere->AttachToComponent(Root, FAttachmentTransformRules::KeepRelativeTransform);
        }
    }
    DetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &UProximityInteraction::OnBeginOverlap);
    DetectionSphere->OnComponentEndOverlap.AddDynamic(this, &UProximityInteraction::OnEndOverlap);
}
void UProximityInteraction::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    DetectionSphere->OnComponentBeginOverlap.RemoveDynamic(this, &UProximityInteraction::OnBeginOverlap);
    DetectionSphere->OnComponentEndOverlap.RemoveDynamic(this, &UProximityInteraction::OnEndOverlap);

    Super::EndPlay(EndPlayReason);
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

void UProximityInteraction::DetectCandidates()
{
    // candidates already tracked by overlap events
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