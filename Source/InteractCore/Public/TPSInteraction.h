// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DefaultInteraction.h"
#include "TPSInteraction.generated.h"

class USphereComponent;
class UEnhancedPlayerInput;

/**
 * Third‑Person interaction component.
 * 
 * Implements interaction behavior designed for third‑person games,
 * usually using character forward traces or camera-assisted targeting.
 */
UCLASS(Blueprintable, BlueprintType, ClassGroup = (InteractCore), meta = (BlueprintSpawnableComponent, DisplayName = "TPS Interaction", Tooltip = "Handles interaction logic for third-person perspectives."))
class INTERACTCORE_API UTPSInteraction : public UDefaultInteraction
{
	GENERATED_BODY()

public:
	UTPSInteraction();

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

private:
	bool IsHoverPressed = false;
	const UEnhancedPlayerInput *EPI = nullptr;

protected:
	UFUNCTION(BlueprintPure, Category = "Interactor|Getter")
	bool IsHoverInputPressed() const;
	virtual bool CanHover(UObject *Interactable) const override;
	virtual void OnControllerReady(AController *InController) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

private:
	UPROPERTY(EditAnywhere, Category = "Interactor|Input")
	bool UseInputForHovering;
	UPROPERTY(EditAnywhere, Category = "Interactor|Input", meta = (EditCondition = "UseInputForHovering == true", EditConditionHides))
	UInputAction *HoverInput;
};
