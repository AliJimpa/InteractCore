// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DefaultInteractor.h"
#include "AreaInteractionComponent.generated.h"

class USphereComponent;
class UEnhancedPlayerInput;

/**
 *
 */
UCLASS(ClassGroup = (Interaction), meta = (BlueprintSpawnableComponent))
class INTERACTCORE_API UAreaInteractionComponent : public UDefaultInteractor
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
