// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DefaultInteraction.h"
#include "MouseInteraction.generated.h"

// UMouseInteractionComponent
/**
 * @class UMouseProjectionInteractionComponent
 * @brief Interaction component for detecting objects via mouse position projected into the 3D world.
 *
 * Features:
 * - Sets the detection pivot point dynamically based on the mouse position.
 * - Projects the 2D mouse position from the screen into the 3D game world to determine interactable objects.
 * - Enables intuitive interaction systems, such as RTS-style unit selection or object picking.
 *
 * Usage:
 * - Add this component to an actor or controller responsible for mouse-based interactions.
 * - Customize projection, distance checks, and interaction logic for your use case.
 */
UCLASS(Blueprintable, BlueprintType, ClassGroup = (InteractCore), meta = (BlueprintSpawnableComponent, DisplayName = "Mouse Interaction", Tooltip = "Handles interaction logic for Point&Ckick style."))
class INTERACTCORE_API UMouseInteraction : public UDefaultInteraction
{
	GENERATED_BODY()

protected:
	virtual void OnControllerReady(AController *InController) override;
	virtual bool TryGetDetectedFocused(FHitResult &OutHit) const override;

private:
	UPROPERTY()
	APlayerController *PC;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Setting")
	float TraceDistance = 10000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Setting")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Setting")
	bool bTraceComplex = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Debug")
	bool bDrawDebug = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Debug")
	FColor DebugColor = FColor::Green;
};
