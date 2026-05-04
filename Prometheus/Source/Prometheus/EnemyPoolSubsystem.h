// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "EnemyPawn.h"
#include "EnemyPoolSubsystem.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FEnemyPoolArray
{
	GENERATED_BODY()

	UPROPERTY(Transient)
	TArray<AEnemyPawn*> InactiveEnemies;
	
};
UCLASS()
class PROMETHEUS_API UEnemyPoolSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

private:
	// Dictionary mapping an Enemy Class to its specific pool array
	UPROPERTY(Transient)
	TMap<TSubclassOf<AEnemyPawn>, FEnemyPoolArray> PoolMap;

public:

	// Pulls an enemy from the pool if available, otherwise spawns a brand new one
	UFUNCTION(BlueprintCallable, Category = "Pooling")
	AEnemyPawn* RequestEnemy(TSubclassOf<AEnemyPawn> EnemyClass, const FTransform& SpawnTransform);

	// Puts a dead enemy back into the pool to be reused later
	UFUNCTION(BlueprintCallable, Category = "Pooling")
	void ReturnEnemy(AEnemyPawn* EnemyToReturn);

	//Reference for the enemy spawn parameters
	FActorSpawnParameters SpawnParameters;
	
	
};
