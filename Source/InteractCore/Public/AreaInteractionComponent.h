// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractionComponent.h"
#include "AreaInteractionComponent.generated.h"

class USphereComponent;

/**
 *
 */
UCLASS(ClassGroup = (Interaction), meta = (BlueprintSpawnableComponent))
class INTERACTCORE_API UAreaInteractionComponent : public UInteractionComponent
{
	GENERATED_BODY()

public:
	UAreaInteractionComponent();

protected:
	void BeginPlay2();

	void DetectCandidates();

	UPROPERTY()
	USphereComponent *DetectionSphere;

	UFUNCTION()
	void OnBeginOverlap(
		UPrimitiveComponent *OverlappedComponent,
		AActor *OtherActor,
		UPrimitiveComponent *OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult &SweepResult);

	UFUNCTION()
	void OnEndOverlap(
		UPrimitiveComponent *OverlappedComponent,
		AActor *OtherActor,
		UPrimitiveComponent *OtherComp,
		int32 OtherBodyIndex);

	UPROPERTY()
	TArray<AActor *> CandidateActors;
};
