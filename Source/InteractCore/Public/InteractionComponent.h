// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractDebug.h"
#include "InteractionComponent.generated.h"

class IInteractable;
class UInputAction;
struct FInputActionInstance;
class UEnhancedInputComponent;

UENUM()
enum class EInteractionSearchMode : uint8
{
	ActorOnly UMETA(
		DisplayName = "Actor Only",
		ToolTip = "Only checks the hit Actor for the interaction interface."),

	ComponentOnly UMETA(
		DisplayName = "Component Only",
		ToolTip = "Only checks the hit Components for the interaction interface."),

	ActorAndComponent UMETA(
		DisplayName = "Actor And Component",
		ToolTip = "Checks both the hit Actor and its Components for the interaction interface."),

	PreferActorFallbackToComponent UMETA(
		DisplayName = "Prefer Actor, Fallback To Component",
		ToolTip = "Checks the Actor first. If the Actor does not implement the interface, searches Components for a valid implementation.")
};

UCLASS(Abstract, Blueprintable, ClassGroup = (InteractCore), meta = (BlueprintSpawnableComponent))
class INTERACTCORE_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInteractionComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	/**
	 * Called when a controller associated with this component becomes available.
	 *
	 * Base implementation caches the controller. Child classes can override this
	 * to perform controller-dependent setup (input bindings, UI, etc.).
	 *
	 * @param InController Controller associated with this component's owner. May be null in some cases.
	 */
	virtual void OnControllerReady(AController *InController) PURE_VIRTUAL(UInteractionComponent::OnControllerReady,);
	virtual bool TryGetDetectedFocused(FHitResult &OutHit) const PURE_VIRTUAL(UInteractionComponent::TryGetDetectedFocused, return false;);
	virtual bool CanHover(UObject *Interactable) const PURE_VIRTUAL(UDefaultInteractor::CanHover, return true;);
	virtual bool CanInteract(UObject *Interactable) const PURE_VIRTUAL(UDefaultInteractor::CustomAdaptiveTick, return true;);

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

private:
	TScriptInterface<IInteractable> CurrentInteractable = nullptr;
	void UpdateInteraction();
	TScriptInterface<IInteractable> ResolveInteractableFromHit(const FHitResult &Hit) const;

	AController *ResolveControllerFromOwnership() const;

	UPROPERTY()
	TObjectPtr<USceneComponent> PivotComponent;
	FTransform PivotValue = FTransform::Identity;

	void SetupInteractionInput(AController *Controller);
	void BindInteractionInput(UEnhancedInputComponent *EIC);
	void OnInteractInput(const FInputActionInstance &Instance);

public:
	UFUNCTION(BlueprintPure, Category = "Interaction|Getter")
	EInteractionSearchMode GetMode() const { return DetectionMode; }
	UFUNCTION(BlueprintPure, Category = "Interaction|Getter")
	const TScriptInterface<IInteractable> GetCurrentInteractable() const { return CurrentInteractable; }
	/**
	 * Returns the transform used as the interaction pivot.
	 *
	 * If a PivotComponent is assigned, its world transform is returned.
	 * Otherwise, the manually assigned PivotValue is used.
	 *
	 * This transform determines where interaction traces, UI prompts,
	 * or gameplay logic should originate from.
	 *
	 * @return World-space transform representing the interaction pivot.
	 */
	UFUNCTION(BlueprintCallable, Category = "Interaction|Getter", meta = (DisplayName = "Get Interaction Pivot", ToolTip = "Returns the world transform used as the interaction pivot. Uses PivotComponent if set, otherwise the manually assigned PivotValue."))
	FTransform GetPivot() const;
	/**
	 * Sets the interaction pivot to use a SceneComponent.
	 *
	 * When set, the pivot will dynamically follow this component's
	 * world transform.
	 *
	 * @param InComponent SceneComponent to use as the pivot source.
	 */
	UFUNCTION(BlueprintCallable, Category = "Interaction|Setter", meta = (DisplayName = "Set Pivot (Component)", ToolTip = "Sets a SceneComponent as the pivot source. The pivot will follow this component's world transform."))
	void SetPivotToComponent(USceneComponent *InComponent);
	/**
	 * Sets a fixed transform as the interaction pivot.
	 *
	 * This clears any assigned PivotComponent and uses the provided
	 * transform as a static pivot value.
	 *
	 * @param InValue World-space transform to use as the pivot.
	 */
	UFUNCTION(BlueprintCallable, Category = "Interaction|Setter", meta = (DisplayName = "Set Pivot (Transform)", ToolTip = "Sets a fixed world transform as the pivot. Clears any assigned PivotComponent."))
	void SetPivotToTransform(const FTransform &InValue);
	UFUNCTION(BlueprintCallable, Category = "Interaction|Setter")
	void SetInteractionActive(bool bEnable);

private:
	UPROPERTY(EditAnywhere, Category = "Interaction")
	EInteractionSearchMode DetectionMode = EInteractionSearchMode::ActorAndComponent;
	UPROPERTY(EditAnywhere, Category = "Interaction|Input")
	bool bInteractionInputBound = true;
	UPROPERTY(EditAnywhere, Category = "Interaction|Input", meta = (EditCondition = "bInteractionInputBound == true", EditConditionHides))
	UInputAction *InteractionInput;
};
