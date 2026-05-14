// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractDebug.h"
#include "InteractorN.generated.h"

USTRUCT(BlueprintType)
struct FInteractionRecord
{
	GENERATED_BODY()

	UPROPERTY()
	FHitResult Hit;

	UPROPERTY()
	bool bHovered = false;

	FORCEINLINE void SetHit(const FHitResult hit)
	{
		if (bHovered == false)
		{
			Hit = hit;
		}
		else
		{
			LOG_ERROR("You can Add More");
			PRINT("You can Add More");
		}
	}

	FORCEINLINE void Clear()
	{
		LOG("Clear Record");
	}
};

UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class INTERACTCORE_API UInteractorN : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInteractorN();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void DoSomething() PURE_VIRTUAL(UInteractorN::DoSomething, );

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

private:
	bool bHasBPTrace = false;
	FInteractionRecord CurrentDetectionData;
	FInteractionRecord ActiveHoverData;
	bool PerformTrace(FInteractionRecord &DetectionData);
	bool EvaluateTraceHits(const FInteractionRecord &DetectionData, FInteractionRecord HoverData);
	int ChooseInteractionTarget(const FInteractionRecord &HoverData);
	void UpdateHoveredActors()
	{
		// if(CanHover())
		// {

		// }
	}
	void UpdateUnhoveredActors()
	{
		///
	}

protected:
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	static void AddHIT(UPARAM(ref) FInteractionRecord &Buffer, const FHitResult hit)
	{
		Buffer.SetHit(hit);
	}
	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction")
	bool DoTrace(const FInteractionRecord &Data);
};
