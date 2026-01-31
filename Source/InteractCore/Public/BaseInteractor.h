// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BaseInteractor.generated.h"

struct FInputActionInstance;
class UInputAction;
class IInteractable;
enum class ETriggerEvent : uint8;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class INTERACTCORE_API UBaseInteractor : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UBaseInteractor();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

private:
	AController *ResolveControllerFromOwnership() const;
	void OnTriggered(const FInputActionInstance &Instance);
	void OnStarted(const FInputActionInstance &Instance);
	void OnOngoing(const FInputActionInstance &Instance);
	void OnCanceled(const FInputActionInstance &Instance);
	void OnCompleted(const FInputActionInstance &Instance);

	void OnInteractionCalled(ETriggerEvent Mode) {};

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interactor|Input")
	UInputAction *InputAction;
};
