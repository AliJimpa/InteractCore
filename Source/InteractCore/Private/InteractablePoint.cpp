// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractablePoint.h"

UInteractablePoint::UInteractablePoint()
{
    bIsImplememtWidgetSettings = GetClass()->IsFunctionImplementedInScript(GET_FUNCTION_NAME_CHECKED(UInteractablePoint, K2_ApplyWidgetSettings));
    PRINT("Point");
}

void UInteractablePoint::BeginPlay()
{
    Super::BeginPlay();

    // 1. Create the WidgetComponent and attach it
    WidgetComponent = NewObject<UWidgetComponent>(this, UWidgetComponent::StaticClass(), TEXT("WidgetComponent"));
    WidgetComponent->SetupAttachment(this);
    WidgetComponent->RegisterComponent();
    ApplyWidgetSettings(WidgetComponent);
    if (bIsImplememtWidgetSettings)
        K2_ApplyWidgetSettings(WidgetComponent);
}

void UInteractablePoint::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UInteractablePoint::ApplyWidgetSettings(UWidgetComponent *widgetComp)
{
    // 2. Configure the WidgetComponent
    WidgetComponent->SetWidgetSpace(WidgetSpace); // or EWidgetSpace::Screen
    WidgetComponent->SetDrawSize(WidgetDrawSize);
    WidgetComponent->SetBlendMode(WidgetBlendMode);

    // Manually create the widget and assign it directly
    if (IndicatorClass)
    {
        UUserWidget *MyWidget = CreateWidget<UUserWidget>(GetWorld(), IndicatorClass);
        if (MyWidget)
        {
            WidgetComponent->SetWidget(MyWidget); // assign instance directly

            // Check if widget implements the interface
            if (MyWidget->GetClass()->ImplementsInterface(UInteractionIndicator::StaticClass()))
            {
                Indicator.SetObject(MyWidget);
                Indicator.SetInterface(Cast<IInteractionIndicator>(MyWidget));
                UE_LOG(LogTemp, Log, TEXT("[%s] Widget implements IInteractionIndicator — stored."), *GetName());
            }
            else
            {
                // Print to screen + log
                const FString ErrorMsg = FString::Printf(
                    TEXT("[%s] Widget class '%s' does NOT implement IInteractionIndicator!"),
                    *GetName(),
                    *IndicatorClass->GetName());

                UE_LOG(LogTemp, Error, TEXT("%s"), *ErrorMsg);

                if (GEngine)
                {
                    GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, ErrorMsg);
                }
            }
        }
    }
}