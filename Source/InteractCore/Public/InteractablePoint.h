// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableZone.h"
#include "Components/WidgetComponent.h"
#include "InteractionIndicator.h"
#include "InteractablePoint.generated.h"

/**
 *
 */
UCLASS()
class INTERACTCORE_API UInteractablePoint : public UInteractableZone
{
	GENERATED_BODY()
public:
	UInteractablePoint();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

protected:
	virtual void ApplyWidgetSettings(UWidgetComponent *widgetComp);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Widget")
	TSubclassOf<class UUserWidget> IndicatorClass;

private:
	UPROPERTY()
	class UWidgetComponent *WidgetComponent;
	UPROPERTY()
	TScriptInterface<IInteractionIndicator> Indicator;
	UPROPERTY()
	bool bIsImplememtWidgetSettings = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|Widget", meta = (AllowPrivateAccess = "true"))
	EWidgetSpace WidgetSpace = EWidgetSpace::World;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|Widget", meta = (AllowPrivateAccess = "true"))
	FVector2D WidgetDrawSize = FVector2D(800.f, 600.f);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|Widget", meta = (AllowPrivateAccess = "true"))
	EWidgetBlendMode WidgetBlendMode = EWidgetBlendMode::Transparent;

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction|Override", meta = (DisplayName = "ApplyWidgetSettings"))
	void K2_ApplyWidgetSettings(UWidgetComponent *widgetComp) const;
	UFUNCTION(BlueprintPure, Category = "Interaction|Getter")
	TScriptInterface<IInteractionIndicator> GetIndicator() const { return Indicator; }
};
