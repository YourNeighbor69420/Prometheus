// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "PrometheusGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class PROMETHEUS_API UPrometheusGameInstance : public UGameInstance
{
	GENERATED_BODY()


public:

	UPROPERTY(BlueprintReadWrite, Category = "Checkpoints")
	bool bHasSavedCheckpoint = false;
	
	UPROPERTY(BlueprintReadWrite, Category = "Checkpoints")
	FVector SavedLocation;

	UPROPERTY(BlueprintReadWrite, Category = "Checkpoints")
	FRotator SavedRotation;
};
