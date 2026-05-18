// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractionComponent.h"
#include "FocusedInteractionComponent.generated.h"

/**
 *
 */
UCLASS()
class INTERACTCORE_API UFocusedInteractionComponent : public UInteractionComponent
{
	GENERATED_BODY()

protected:
	virtual void OnControllerReady(AController *InController) override;
	virtual bool TryGetDetectedFocused(FHitResult &OutHit) const override;
};
