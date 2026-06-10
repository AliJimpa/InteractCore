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
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	virtual void SetupZone(USphereComponent *Zone) const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, AdvancedDisplay, Category = "Interaction", meta = (BlueprintProtected))
	USphereComponent *SphereZone;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|DetectionZone", meta = (AllowPrivateAccess = "true"))
	FVector ZoneOffset = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|DetectionZone", meta = (AllowPrivateAccess = "true"))
	float ZoneRadius = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|DetectionZone", meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<ECollisionEnabled::Type> ZoneCollision = ECollisionEnabled::QueryAndPhysics;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|DetectionZone", meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<ECollisionChannel> ZoneCollisionObjectType = ECC_WorldDynamic;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|DetectionZone", meta = (AllowPrivateAccess = "true"))
	FCollisionProfileName ZoneCollisionProfile = FCollisionProfileName(TEXT("OverlapAll"));

private:
	bool bIsImplememtSetupZone = false;

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction|Override", meta = (DisplayName = "SetupZone"))
	void K2_SetupZone(USphereComponent *Zone) const;

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
