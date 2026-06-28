#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractionIndicator.generated.h"

UENUM(BlueprintType)
enum class EInteractionState : uint8
{
    Begindetection UMETA(DisplayName = "Begin Detection"),
    Enddetection UMETA(DisplayName = "End Detection"),
    Beginhover UMETA(DisplayName = "Begin Hover"),
    Endhover UMETA(DisplayName = "End Hover")
};

// UINTERFACE is required, but no Blueprint exposure
UINTERFACE(Blueprintable, BlueprintType)
class INTERACTCORE_API UInteractionIndicator : public UInterface
{
    GENERATED_BODY()
};

// Pure C++ interface
class INTERACTCORE_API IInteractionIndicator
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "InteractionIndicator|Events", meta = (ToolTip = "Initializes the interaction indicator with its owning interactable component."))
    void InitializeIndicator(UObject *InOwner);
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "InteractionIndicator|Events", meta = (ToolTip = "Called whenever the interaction state changes."))
    void OnInteractionStateChanged(EInteractionState NewState);
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "InteractionIndicator|Events", meta = (ToolTip = "Called to update the current interaction progress."))
    void OnInteractionProgress(float Progress);
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "InteractionIndicator|Input", meta = (ToolTip = "Called when the interaction has been successfully completed.."))
    void OnInteractionCompleted();
};
