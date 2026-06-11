// InteractableWidgetInterface.h
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractionIndicator.generated.h"

UINTERFACE(Blueprintable, BlueprintType)
class INTERACTCORE_API UInteractionIndicator : public UInterface
{
    GENERATED_BODY()
};

class INTERACTCORE_API IInteractionIndicator
{
    GENERATED_BODY()

public:
    // Called when player interacts with the sphere component
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interactable|Events")
    void TEST(UInteractionComponent *Provider);
};