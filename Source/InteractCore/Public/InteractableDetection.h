// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableComponent.h"
#include "InteractableDetection.generated.h"

/**
 *
 */
UCLASS(Blueprintable, BlueprintType, ClassGroup = (InteractCore), meta = (BlueprintSpawnableComponent, DisplayName = "InteractableDetection", Tooltip = "Handel Interactable Collider"))
class INTERACTCORE_API UInteractableDetection : public UInteractableComponent
{
	GENERATED_BODY()
public:
	UInteractableDetection();

protected:
	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	virtual void ApplyZoneSettings(USphereComponent *Spher) const;
	virtual void OnInteractorDetected(UInteractionComponent *Interactor);
	virtual void OnInteractorLost(UInteractionComponent *Interactor);

public:
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnInteractionEvent OnZoneBegin;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnInteractionEvent OnZoneEnd;

protected:
	UPROPERTY(VisibleAnywhere, AdvancedDisplay, Category = "Interaction", meta = (BlueprintProtected))
	USphereComponent *DetectionComponent = nullptr;

private:
	UPROPERTY()
	bool bIsImplememtZoneSettings = false;
	UPROPERTY()
	UInteractionComponent *DetectedObj;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|DetectionSphere", meta = (AllowPrivateAccess = "true"))
	FVector DetectionOffset = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|DetectionSphere", meta = (AllowPrivateAccess = "true"))
	float DetectionRadius = 500.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|DetectionSphere", meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<ECollisionEnabled::Type> DetectionCollision = ECollisionEnabled::QueryAndPhysics;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|DetectionSphere", meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<ECollisionChannel> DetectionObjectType = ECC_WorldDynamic;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|DetectionSphere", meta = (AllowPrivateAccess = "true"))

	FCollisionProfileName DetectionProfile = FCollisionProfileName(TEXT("OverlapAll"));
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|FirstCheck", meta = (ToolTip = "If enabled, at the beginplay add sphere trace like DetectionSpere for begin check", AllowPrivateAccess = "true"))
	bool bFirstCheck = true;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|FirstCheck", meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<ECollisionChannel> DetectionChannel = ECC_Camera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|DynamicRadius", meta = (ToolTip = "If enabled, the InteractableSphere radius is dynamically adjusted every Tick based on the distance between this actor and the character.", AllowPrivateAccess = "true"))
	bool bDynamicRadius = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|DynamicRadius", meta = (AllowPrivateAccess = "true"))
	float DistanceThreshold = 100;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|DynamicRadius", meta = (AllowPrivateAccess = "true"))
	FVector2D RadiusRange = FVector2D::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|ToggleRadius", meta = (ToolTip = "If enabled, the DetectionSphere radius change when agent begin overlap and end", AllowPrivateAccess = "true"))
	bool bToggleRadius = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|ToggleRadius", meta = (AllowPrivateAccess = "true"))
	float ToggleRadius = 700.f;

public:
	UFUNCTION(BlueprintPure, Category = "Interaction|Getter")
	UInteractionComponent *GetCurrentDetected() const { return DetectedObj; }
	UFUNCTION(BlueprintPure, Category = "Interaction|Status")
	bool IsDetected() const { return DetectedObj != nullptr; }

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction|Override", meta = (DisplayName = "ApplyZoneSettings"))
	void K2_ApplyZoneSettings(USphereComponent *Zone) const;
	UFUNCTION(BlueprintNativeEvent, Category = "Interactable|Override")
	void FirstCheck(float radius , ECollisionChannel Channel);

private:
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent *OverlappedComp, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent *OverlappedComp, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex);
#if WITH_EDITOR
	UFUNCTION(CallInEditor, Category = "Interaction", meta = (DisplayName = "DetectionSphere"))
	void ShowDebug();
#endif
};
