// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DefaultInteraction.h"
#include "FPSInteraction.generated.h"

UENUM()
enum class EInteractionTraceAxis : uint8
{
	Forward UMETA(DisplayName = "Forward (X)"),
	Right UMETA(DisplayName = "Right (Y)"),
	Up UMETA(DisplayName = "Up (Z)")
};
UENUM()
enum class EInteractionTraceType : uint8
{
	LineTrace,
	SphereTrace,
	Auto,
};

/**
 * First‑Person interaction component.
 *
 * Implements interaction behavior designed for first‑person games,
 * typically using camera-based traces and direct player view targeting.
 */
UCLASS(Blueprintable, BlueprintType, ClassGroup = (InteractCore), meta = (BlueprintSpawnableComponent, DisplayName = "FPS Interaction", Tooltip = "Handles interaction logic for first-person perspectives."))
class INTERACTCORE_API UFPSInteraction : public UDefaultInteraction
{
	GENERATED_BODY()

public:
	UFPSInteraction();

protected:
	virtual void OnControllerReady(AController *InController) override;
	virtual bool TryGetDetectedFocused(FHitResult &OutHit) const override;

private:
	bool PerformInteractionTrace(FHitResult &OutHit) const;
	FVector GetTraceDirection(const FTransform &Pivot) const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|FPS")
	EInteractionTraceType TraceType = EInteractionTraceType::Auto;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|FPS", meta = (BlueprintProtected))
	FVector Offcet = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|FPS", meta = (ClampMin = "100", BlueprintProtected))
	float TraceDistance = 500.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|FPS", meta = (BlueprintProtected))
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|FPS", meta = (BlueprintProtected))
	EInteractionTraceAxis TraceAxis = EInteractionTraceAxis::Forward;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|FPS", meta = (EditCondition = "TraceType == EInteractionTraceType::SphereTrace || TraceType == EInteractionTraceType::Auto", EditConditionHides))
	TArray<float> SphereTraceRadii = {5.f, 10.f, 15.f}; // 3 levels

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|Debug")
	bool bDrawDebugTrace = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|Debug")
	FColor DebugHitColor = FColor::Green;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|Debug")
	FColor DebugNoHitColor = FColor::Red;
};
