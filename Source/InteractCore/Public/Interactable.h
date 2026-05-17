#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactable.generated.h"

class UInteractor;

// UINTERFACE is required, but no Blueprint exposure
UINTERFACE(Blueprintable)
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
    void Interact(UActorComponent *Provider, FHitResult Hit);
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interactable|Permisions")
    bool CanHover() const;
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interactable|Permisions")
    bool CanInteract(ETriggerEvent InputEventType, const FInputActionInstance &InputValue) const;
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interactable|Settings")
    int32 GetPriority() const;
};
