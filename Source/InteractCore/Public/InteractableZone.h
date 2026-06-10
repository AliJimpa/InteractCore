// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableComponent.h"
#include "InteractableZone.generated.h"

/**
 *
 */
UCLASS(Blueprintable, BlueprintType, ClassGroup = (InteractCore), meta = (BlueprintSpawnableComponent, DisplayName = "InteractableZone", Tooltip = "Handel Interactable Collider"))
class INTERACTCORE_API UInteractableZone : public UInteractableComponent
{
	GENERATED_BODY()
public:
	UInteractableZone();

protected:
	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	virtual void ApplyZoneSettings(USphereComponent *Zone) const;
	virtual void OnInteractorDetected(UInteractionComponent *Interactor) {}
	virtual void OnInteractorLost(UInteractionComponent *Interactor) {}

private:
	bool CheckLineOfSight() const;

public:
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnInteractionEvent OnZoneBegin;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnInteractionEvent OnZoneEnd;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, AdvancedDisplay, Category = "Interaction", meta = (BlueprintProtected))
	USphereComponent *SphereZone;

private:
	UPROPERTY()
	bool bIsImplememtZoneSettings = false;
	UPROPERTY()
	UInteractionComponent *DetectedObj;
	UPROPERTY()
	bool bCanSee = false; // that means the component can see target object detected by zone
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|DetectionZone", meta = (AllowPrivateAccess = "true"))
	FVector ZoneOffset = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|DetectionZone", meta = (AllowPrivateAccess = "true"))
	float ZoneRadius = 500.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|DetectionZone", meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<ECollisionEnabled::Type> ZoneCollision = ECollisionEnabled::QueryAndPhysics;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|DetectionZone", meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<ECollisionChannel> ZoneCollisionObjectType = ECC_WorldDynamic;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|DetectionZone", meta = (AllowPrivateAccess = "true"))
	FCollisionProfileName ZoneCollisionProfile = FCollisionProfileName(TEXT("OverlapAll"));
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|Sight", meta = (Tooltip = "When true, if target is inside the detection sphere and line-of-sight trace will run each Tick.", AllowPrivateAccess = "true"))
	bool bCheckLineOfSight = true;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|Sight", meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<ECollisionChannel> SightChannel = ECC_Visibility;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|Sight", meta = (AllowPrivateAccess = "true"))
	bool bShowDebugSight = false;

public:
	UFUNCTION(BlueprintPure, Category = "Interaction|Status")
	bool IsInZone() const { return DetectedObj != nullptr; }

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction|Override", meta = (DisplayName = "ApplyZoneSettings"))
	void K2_ApplyZoneSettings(USphereComponent *Zone) const;

private:
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent *OverlappedComp, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent *OverlappedComp, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex);
#if WITH_EDITOR
	UFUNCTION(CallInEditor, Category = "Interaction")
	void ShowDebug();
#endif
};
