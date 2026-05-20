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
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interactable|Events", meta = (ToolTip = "Need Tip"))
    void Hover(UActorComponent *Provider, FHitResult Hit);
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interactable|Events")
    void UnHover(UActorComponent *Provider);
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interactable|Events")
    void Interact(UActorComponent *Provider);
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interactable|Input")
    bool ShouldHandleInput(const FInputActionInstance &InputValue) const;
};
