// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MarkableComponent.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "PlayerSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class PROMETHEUS_API UPlayerSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

	//Set the current target
	UFUNCTION(BlueprintCallable)
	void SetMarkedTarget(UMarkableComponent* NewMarkedTarget);

	//Wipes current target
	UFUNCTION(BlueprintCallable)
	void ClearMarkedTarget();

	//Returns the current marked target
	UFUNCTION(BlueprintCallable)
	UMarkableComponent* GetMarkedTarget() const;

	
	
private:
	//Weak object pointer used in order to avoid crashing if target is destroyed before its cleared
	TWeakObjectPtr<UMarkableComponent> CurrentMarkedTarget;
};
