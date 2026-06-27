#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactable.generated.h"

// UINTERFACE is required, but no Blueprint exposure
UINTERFACE(Blueprintable, BlueprintType)
class INTERACTCORE_API UInteractable : public UInterface
{
    GENERATED_BODY()
};

// Pure C++ interface
class INTERACTCORE_API IInteractable
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interactable|Events", meta = (ToolTip = "Called when an interaction provider starts hovering this object."))
    void Hover(UInteractionComponent *Provider, const FHitResult &Hit);
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interactable|Events", meta = (ToolTip = "Called when an interaction provider stops hovering this object."))
    void UnHover(UInteractionComponent *Provider);
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interactable|Events", meta = (ToolTip = "Called when an interaction is performed on this object."))
    void Interact(UInteractionComponent *Provider, const FHitResult &Hit, const FInputActionInstance &Instance);
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interactable|Input", meta = (ToolTip = "Determines whether this interactable should handle the given input action."))
    bool ShouldHandleInput(const FInputActionInstance &InputValue) const;
};
