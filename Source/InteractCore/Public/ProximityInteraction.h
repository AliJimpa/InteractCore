// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DefaultInteraction.h"
#include "ProximityInteraction.generated.h"

class USphereComponent;
class UEnhancedPlayerInput;

// ProximityInteractionComponent
/**
 * @class UProximityInteraction
 * @brief Interaction component for detecting objects around the owner actor using collision and distance.
 *
 * Features:
 * - Sets the detection pivot at the owner's actor position.
 * - Uses collision queries to find interactable objects around the owner.
 * - Includes distance-based checks to refine target detection.
 * - Ideal for NPCs, vehicles, or player characters detecting nearby interactables.
 *
 * Usage:
 * - Attach this component to any actor to enable proximity-based interaction detection.
 * - Configure collision profiles and distance thresholds for your gameplay scenario.
 */
UCLASS(Blueprintable, BlueprintType, ClassGroup = (InteractCore), meta = (BlueprintSpawnableComponent, DisplayName = "TPS Interaction", Tooltip = "Handles interaction logic for third-person perspectives."))
class INTERACTCORE_API UProximityInteraction : public UDefaultInteraction
{
	GENERATED_BODY()

public:
	UProximityInteraction();

protected:
	virtual void BeginPlay() override;
	virtual void OnRegister() override;
	virtual void OnControllerReady(AController *InController) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	virtual bool TryGetDetectedFocused(FHitResult &OutHit) const override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent &PropertyChangedEvent) override;
#endif

private:
	const UEnhancedPlayerInput *EPI = nullptr;
	void UpdateCollisionSettings();

private:
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);
	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex);
	UFUNCTION(BlueprintPure, Category = "Interactor|Getter")
	bool IsHoverInputPressed() const;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Debug")
	bool bDrawDebugSphere = true;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction|Setting")
	USphereComponent *DetectionSphere;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction|Setting")
	TArray<FHitResult> CandidateHits;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Collision", meta = (ClampMin = "0.0"))
	float DetectionRadius = 300.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Collision")
	TEnumAsByte<ECollisionEnabled::Type> CollisionEnabled = ECollisionEnabled::QueryOnly;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Collision")
	TEnumAsByte<ECollisionChannel> CollisionChannel = ECC_Pawn;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Collision")
	TEnumAsByte<ECollisionResponse> DefaultResponse = ECR_Ignore;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Collision")
	bool bGenerateOverlapEvents = true;

private:
	UPROPERTY(EditAnywhere, Category = "Interactor|Input")
	bool UseInputForHovering;
	UPROPERTY(EditAnywhere, Category = "Interactor|Input", meta = (EditCondition = "UseInputForHovering == true", EditConditionHides))
	UInputAction *HoverInput;
};
