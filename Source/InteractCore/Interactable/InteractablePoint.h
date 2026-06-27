// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableDetection.h"
#include "Components/WidgetComponent.h"
#include "Widget/InteractionIndicatorWidget.h"
#include "InteractablePoint.generated.h"

/**
 *
 */
UCLASS(Blueprintable, BlueprintType, ClassGroup = (InteractCore), meta = (BlueprintSpawnableComponent, DisplayName = "InteractablePoint", Tooltip = "Handel Interactable Indicator"))
class INTERACTCORE_API UInteractablePoint : public UInteractableDetection
{
	GENERATED_BODY()
public:
	UInteractablePoint();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	virtual void ApplyWidgetSettings(UWidgetComponent *widgetComp);
	virtual void Interact_Implementation(UInteractionComponent *Provider, const FHitResult &Hit, const FInputActionInstance &Instance) override;
	virtual void Hover_Implementation(UInteractionComponent *Provider, const FHitResult &Hit) override;
	virtual void UnHover_Implementation(UInteractionComponent *Provider) override;
	virtual void OnInteractorDetected(UInteractionComponent *Interactor) override;
	virtual void OnInteractorLost(UInteractionComponent *Interactor) override;
	virtual bool ShouldHandleInput_Implementation(const FInputActionInstance &InputValue) const override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent &PropertyChangedEvent) override;
#endif

private:
	bool CheckLineOfSight(UInteractionComponent *detectedObj) const;

protected:
	UPROPERTY(VisibleAnywhere, AdvancedDisplay, Category = "Interaction", meta = (BlueprintProtected))
	class UWidgetComponent *IndicatorComponent;

private:
	UPROPERTY()
	TScriptInterface<IInteractionIndicator> Indicator = nullptr;
	UPROPERTY()
	bool bIsImplememtWidgetSettings = false;
	UPROPERTY()
	bool bCanSee = false; // that means the component can see target object detected by zone
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|WidgetIndicator", meta = (AllowPrivateAccess = "true", MustImplement = "/Script/InteractCore.InteractionIndicator"))
	TSubclassOf<UUserWidget> IndicatorClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|WidgetIndicator", meta = (AllowPrivateAccess = "true"))
	EWidgetSpace WidgetSpace = EWidgetSpace::Screen;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|WidgetIndicator", meta = (AllowPrivateAccess = "true"))
	EWidgetBlendMode WidgetBlendMode = EWidgetBlendMode::Transparent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|WidgetIndicator", meta = (AllowPrivateAccess = "true"))
	FVector2D WidgetDrawSize = FVector2D(250.f, 250.f);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|WidgetIndicator", meta = (AllowPrivateAccess = "true"))
	FVector WidgetOffset = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|Sight", meta = (Tooltip = "When true, if target is inside the detection sphere and line-of-sight trace will run each Tick.", AllowPrivateAccess = "true"))
	bool bCheckLineOfSight = true;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|Sight", meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<ECollisionChannel> SightChannel = ECC_Camera;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|Sight", meta = (AllowPrivateAccess = "true"))
	bool bShowDebugSight = false;

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction|Override", meta = (DisplayName = "ApplyWidgetSettings"))
	void K2_ApplyWidgetSettings(UWidgetComponent *widgetComp) const;
	UFUNCTION(BlueprintPure, Category = "Interaction|Getter")
	TScriptInterface<IInteractionIndicator> GetIndicator() const { return Indicator; }
	UFUNCTION(BlueprintPure, Category = "Interaction|Status")
	bool CanSeeDetectedObject() const { return IsDetected() ? bCanSee : false; }
};
