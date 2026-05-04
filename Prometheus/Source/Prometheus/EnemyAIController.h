// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "DetourCrowdAIController.h"
#include "EnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class PROMETHEUS_API AEnemyAIController : public ADetourCrowdAIController
{
	GENERATED_BODY()

	//When AI Controller takes over a pawn
	virtual void OnPossess(APawn* InPawn) override;
};
