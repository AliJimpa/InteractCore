// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactor.h"
#include "DefaultInteractor.generated.h"

/**
 *
 */
UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class INTERACTCORE_API UDefaultInteractor : public UInteractor
{
	GENERATED_BODY()

protected:
	virtual bool PerformSingleTrace(FInteractionData &OutData) override;
	virtual bool PerformMultiTrace(FInteractionData &OutData) override;
	virtual const FInteractionRecord *SelectInteractionTarget(const FInteractionData &HoverData) override;
};
