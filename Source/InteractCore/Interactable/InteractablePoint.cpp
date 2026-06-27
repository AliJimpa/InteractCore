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

    if (IndicatorClass)
    {
        checkf(
            IndicatorClass->ImplementsInterface(UInteractionIndicator::StaticClass()),
            TEXT("IndicatorClass must implement UInteractionIndicator"));
    }

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
    IInteractionIndicator::Execute_OnInteractionCompleted(Indicator.GetObject());
}
void UInteractablePoint::Hover_Implementation(UInteractionComponent *Provider, const FHitResult &Hit)
{
    Super::Hover_Implementation(Provider, Hit);
    IInteractionIndicator::Execute_OnInteractionStateChanged(Indicator.GetObject(),EInteractionState::Beginhover);
}
void UInteractablePoint::UnHover_Implementation(UInteractionComponent *Provider)
{
    Super::UnHover_Implementation(Provider);
    IInteractionIndicator::Execute_OnInteractionStateChanged(Indicator.GetObject(),EInteractionState::Endhover);
}
void UInteractablePoint::OnInteractorDetected(UInteractionComponent *Interactor)
{
    Super::OnInteractorDetected(Interactor);
    IInteractionIndicator::Execute_OnInteractionStateChanged(Indicator.GetObject(),EInteractionState::Begindetection);
}
void UInteractablePoint::OnInteractorLost(UInteractionComponent *Interactor)
{
    Super::OnInteractorLost(Interactor);
    IInteractionIndicator::Execute_OnInteractionStateChanged(Indicator.GetObject(),EInteractionState::Enddetection);
}
bool UInteractablePoint::ShouldHandleInput_Implementation(const FInputActionInstance &InputValue) const
{
    if (InputMode == EInteractionInputMode::Hold || InputMode == EInteractionInputMode::ChargedRelease)
    {
        const float progress = FMath::Clamp(InputValue.GetElapsedTime() / HoldTimeThreshold, 0.f, 1.f);
        IInteractionIndicator::Execute_OnInteractionProgress(Indicator.GetObject(),progress);
    }
    return Super::ShouldHandleInput_Implementation(InputValue);
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

    static const UClass *InteractionIndicatorClass = UInteractionIndicator::StaticClass();
    auto MakeInteractionIndicator = [](UObject *Object) -> TScriptInterface<IInteractionIndicator>
    {
        TScriptInterface<IInteractionIndicator> Result = nullptr;

        if (!Object)
        {
            return Result;
        }

        if (Object->GetClass()->ImplementsInterface(InteractionIndicatorClass))
        {
            Result.SetObject(Object);
            Result.SetInterface(Cast<IInteractionIndicator>(Object));
        }

        return Result;
    };

    UUserWidget *Widget = CreateWidget<UUserWidget>(GetWorld(), IndicatorClass);
    Indicator = MakeInteractionIndicator(Widget);
    if (Indicator)
    {
        // widgetComp->SetWidgetClass(IndicatorClass);
        // widgetComp->InitWidget();
        widgetComp->SetWidget(Widget);
        IInteractionIndicator::Execute_InitializeIndicator(Indicator.GetObject(),this);
    }
    else
    {
        LOG_ERROR("IndicatorClass does not implement UInteractionIndicator")
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("IndicatorClass does not implement UInteractionIndicator"));
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

#if WITH_EDITOR
void UInteractablePoint::PostEditChangeProperty(FPropertyChangedEvent &PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    if (IndicatorClass &&
        !IndicatorClass->ImplementsInterface(UInteractionIndicator::StaticClass()))
    {
        UE_LOG(LogTemp, Error, TEXT("IndicatorClass must implement UInteractionIndicator"));
        IndicatorClass = nullptr;
    }
}
#endif