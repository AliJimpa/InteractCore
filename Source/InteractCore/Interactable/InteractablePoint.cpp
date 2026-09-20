// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractablePoint.h"
#include "Interface/Interactable.h"
#include "Interaction/InteractionComponent.h"
#include "EnhancedInput\Public\InputAction.h"

UInteractablePoint::UInteractablePoint()
{
    bIsImplememtWidgetSettings = GetClass()->IsFunctionImplementedInScript(GET_FUNCTION_NAME_CHECKED(UInteractablePoint, K2_ApplyWidgetSettings));

    static ConstructorHelpers::FObjectFinder<UClass> WidgetClassFinder(TEXT("/InteractCore/Widgets/WBP_InteractionIndicator.WBP_InteractionIndicator_C"));
    if (WidgetClassFinder.Succeeded())
    {
        IndicatorClass = WidgetClassFinder.Object;
    }
    // PRINT("Point");
}

void UInteractablePoint::BeginPlay()
{
    Super::BeginPlay();

    if (!IndicatorComponent)
    {
        IndicatorComponent = NewObject<UWidgetComponent>(this, UWidgetComponent::StaticClass(), MakeUniqueObjectName(this, UWidgetComponent::StaticClass(), TEXT("WidgetComponent")));
        IndicatorComponent->SetupAttachment(this);
        IndicatorComponent->RegisterComponent();
        ApplyWidgetSettings(IndicatorComponent);
        if (bIsImplememtWidgetSettings)
            K2_ApplyWidgetSettings(IndicatorComponent);
    }
}

void UInteractablePoint::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (bCheckLineOfSight)
    {
        UInteractionComponent *obj = GetCurrentDetected();
        if (IsValid(obj))
        {
            bCanSee = CheckLineOfSight(obj);
        }
        else
        {
            bCanSee = false;
        }
    }
}

void UInteractablePoint::Interact_Implementation(UInteractionComponent *Provider, const FHitResult &Hit, const FInputActionInstance &Instance)
{
    Super::Interact_Implementation(Provider, Hit, Instance);
    if (Indicator != nullptr)
    {
        Indicator->OnInteractionCompleted();
    }
}
void UInteractablePoint::Hover_Implementation(UInteractionComponent *Provider, const FHitResult &Hit)
{
    Super::Hover_Implementation(Provider, Hit);
    if (Indicator != nullptr)
    {
        Indicator->OnInteractionStateChanged(EInteractionState::Beginhover);
    }
}
void UInteractablePoint::UnHover_Implementation(UInteractionComponent *Provider)
{
    Super::UnHover_Implementation(Provider);
    if (Indicator != nullptr)
    {
        Indicator->OnInteractionStateChanged(EInteractionState::Endhover);
    }
}
void UInteractablePoint::OnInteractorDetected(UInteractionComponent *Interactor)
{
    Super::OnInteractorDetected(Interactor);
    if (Indicator != nullptr)
    {
        Indicator->OnInteractionStateChanged(EInteractionState::Begindetection);
    }
}
void UInteractablePoint::OnInteractorLost(UInteractionComponent *Interactor)
{
    Super::OnInteractorLost(Interactor);
    if (Indicator != nullptr)
    {
        Indicator->OnInteractionStateChanged(EInteractionState::Enddetection);
    }
}
bool UInteractablePoint::ShouldHandleInput_Implementation(const FInputActionInstance &InputAction) const
{
    if (InputMode == EInteractionInputMode::Hold || InputMode == EInteractionInputMode::ChargedRelease)
    {
        const float progress = FMath::Clamp(InputAction.GetElapsedTime() / HoldTimeThreshold, 0.f, 1.f);

        if (Indicator != nullptr)
        {
            Indicator->OnInteractionProgress(progress);
        }
    }
    return Super::ShouldHandleInput_Implementation(InputAction);
}

bool UInteractablePoint::CheckLineOfSight(UInteractionComponent *detectedObj) const
{
    // Start from this component location
    const FVector Start = GetComponentTransform().GetLocation();
    // Aim for the target to reduce false negatives
    const FVector End = detectedObj->GetOwner()->GetActorLocation();

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(GetOwner()); // ignore self only

    bool bSeen = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, SightChannel, Params);

    // Seen only if the trace hit the target actor directly
    if (bSeen)
        bSeen = Hit.GetActor() == detectedObj->GetOwner();

#if WITH_EDITOR
    if (bShowDebugSight)
        DrawDebugLine(GetWorld(), Start, End, Hit.bBlockingHit ? FColor::Green : FColor::Red, false, 0.f, 0, 1.f);
#endif

    return bSeen;
}

void UInteractablePoint::ApplyWidgetSettings(UWidgetComponent *widgetComp)
{
    widgetComp->SetRelativeLocation(WidgetOffset);
    widgetComp->SetWidgetSpace(WidgetSpace);
    widgetComp->SetDrawSize(WidgetDrawSize);
    widgetComp->SetBlendMode(WidgetBlendMode);
    widgetComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    if (IndicatorClass)
    {
        Indicator = CreateWidget<UInteractionIndicatorWidget>(GetWorld(), IndicatorClass);
        if (Indicator != nullptr)
        {
            Indicator->InitializeIndicator(this);
            widgetComp->SetWidget(Indicator);
        }
        
    }
}

void UInteractablePoint::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    // Unbind overlap events
    if (IndicatorComponent != nullptr)
    {
        IndicatorComponent->DestroyComponent();
        IndicatorComponent = nullptr;
    }

    Super::EndPlay(EndPlayReason);
}
