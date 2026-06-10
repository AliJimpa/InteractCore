// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableZone.h"
#include "InteractablePoint.generated.h"

/**
 *
 */
UCLASS()
class INTERACTCORE_API UInteractablePoint : public UInteractableZone
{
	GENERATED_BODY()
public:
	UInteractablePoint();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
};
