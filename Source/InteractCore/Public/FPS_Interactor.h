// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DefaultInteractor.h"
#include "FPS_Interactor.generated.h"

UENUM(BlueprintType)
enum class EInteractionTraceAxis : uint8
{
	Forward UMETA(DisplayName = "Forward (X)"),
	Right UMETA(DisplayName = "Right (Y)"),
	Up UMETA(DisplayName = "Up (Z)")
};

/**
 *
 */
UCLASS(Blueprintable, ClassGroup = (Core), meta = (BlueprintSpawnableComponent))
class INTERACTCORE_API UFPS_Interactor : public UDefaultInteractor
{
	GENERATED_BODY()

protected:
	virtual void OnControllerReady(AController *InController) override;
	virtual bool TryGetDetectedFocused(FHitResult &OutHit) const override;
	virtual void CustomAdaptiveTick(float Threshould, float &OutTickRate) override;

private:
	/** Performs the interaction trace */
	bool PerformInteractionTrace(FHitResult &OutHit) const;

	/** Returns the axis direction used for tracing */
	FVector GetTraceDirection(const FTransform &Pivot) const;

protected:
	/** Maximum distance of the interaction trace */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|Trace", meta = (ClampMin = "100"))
	float TraceDistance = 500.f;
	/** Collision channel used for the interaction trace */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|Trace")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;
	/** Axis used for trace direction (relative to pivot transform) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|Trace")
	EInteractionTraceAxis TraceAxis = EInteractionTraceAxis::Forward;
	/** Draw debug line for the trace */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|Debug")
	bool bDrawDebugTrace = false;
	/** Color of debug trace when a hit occurs */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|Debug")
	FColor DebugHitColor = FColor::Green;
	/** Color of debug trace when no hit occurs */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|Debug")
	FColor DebugNoHitColor = FColor::Red;
};
