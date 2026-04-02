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
	UPROPERTY(Transient)
	TMap<TSubclassOf<AEnemyPawn>, FEnemyPoolArray> PoolMap;

public:

	UFUNCTION(BlueprintCallable, Category = "Pooling")
	AEnemyPawn* RequestEnemy(TSubclassOf<AEnemyPawn> EnemyClass, const FTransform& SpawnTransform);

	UFUNCTION(BlueprintCallable, Category = "Pooling")
	void ReturnEnemy(AEnemyPawn* EnemyToReturn);

	FActorSpawnParameters SpawnParameters;
	
	
};
