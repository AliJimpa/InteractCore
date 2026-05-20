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
UCLASS()
class INTERACTCORE_API UMouseInteraction : public UDefaultInteraction
{
	GENERATED_BODY()
	
};
