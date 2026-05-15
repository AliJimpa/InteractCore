// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractorN.h"
#include "DefaultInteractor.generated.h"

/**
 *
 */
UCLASS()
class INTERACTCORE_API UDefaultInteractor : public UInteractorN
{
	GENERATED_BODY()

protected:
	virtual bool PerformSingleTrace(FInteractionData &OutData) override;
	virtual bool PerformMultiTrace(FInteractionData &OutData) override;
};
