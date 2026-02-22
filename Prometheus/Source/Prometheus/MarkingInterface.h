// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MarkingInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UMarkingInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROMETHEUS_API IMarkingInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	void OnMarked();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	void OnUnMarked();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	void DealDamage(float DamageAmount);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	float GetHealth();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	float GetMaxHealth();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	float GetSpeedReward();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	float GetExecuteSpeedReward();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	float GetSpeedPenalty();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	float GetDamageThreshold();
	
};
