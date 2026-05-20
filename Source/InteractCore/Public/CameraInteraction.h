// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DefaultInteraction.h"
#include "CameraInteraction.generated.h"

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

// CameraInteraction
/**
 * @class UCameraInteraction
 * @brief Interaction component for detecting objects via camera-based line traces.
 * 
 * Features:
 * - Sets the detection pivot point at the camera position.
 * - Performs interaction detection using line traces from the camera to the targeted object.
 * - Best suited for first-person or third-person camera-based interaction systems.
 * 
 * Usage:
 * - Add this component to an actor to enable camera-based tracing for interactables.
 * - Customize the trace parameters and permissions as needed.
 */
UCLASS(Blueprintable, BlueprintType, ClassGroup = (InteractCore), meta = (BlueprintSpawnableComponent, DisplayName = "Camera Interaction", Tooltip = "Handles interaction logic for first-person perspectives."))
class INTERACTCORE_API UCameraInteraction : public UDefaultInteraction
{
	GENERATED_BODY()

public:
	UCameraInteraction();

protected:
	virtual void OnControllerReady(AController *InController) override;
	virtual bool TryGetDetectedFocused(FHitResult &OutHit) const override;

private:
	bool PerformInteractionTrace(FHitResult &OutHit) const;
	FVector GetTraceDirection(const FTransform &Pivot) const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Setting")
	EInteractionTraceType TraceType = EInteractionTraceType::Auto;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Setting", meta = (BlueprintProtected))
	FVector Offcet = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Setting", meta = (ClampMin = "100", BlueprintProtected))
	float TraceDistance = 500.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Setting", meta = (BlueprintProtected))
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Setting", meta = (BlueprintProtected))
	EInteractionTraceAxis TraceAxis = EInteractionTraceAxis::Forward;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Setting", meta = (EditCondition = "TraceType == EInteractionTraceType::SphereTrace || TraceType == EInteractionTraceType::Auto", EditConditionHides))
	TArray<float> SphereTraceRadii = {5.f, 10.f, 15.f}; // 3 levels

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|Debug")
	bool bDrawDebugTrace = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|Debug")
	FColor DebugHitColor = FColor::Green;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|Debug")
	FColor DebugNoHitColor = FColor::Red;
};
