// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyPoolSubsystem.h"

#include "EnemyAIController.h"

AEnemyPawn* UEnemyPoolSubsystem::RequestEnemy(TSubclassOf<AEnemyPawn> EnemyClass, const FTransform& SpawnTransform)
{
	//Safety Check
	if (!EnemyClass) return nullptr;

	AEnemyPawn* Enemy = nullptr;

	//Always spawn the enemy 
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	//If object pool has an array for the enemy and if it isn't empty
	if (PoolMap.Contains(EnemyClass) && !PoolMap[EnemyClass].InactiveEnemies.IsEmpty())
	{
		//If so, Grab the last inactive enemy from the array
		Enemy = PoolMap[EnemyClass].InactiveEnemies.Pop();
	}
	else
	{
		//If the pool is empty, spawn a new one
		UWorld* World = GetWorld();
		if (World)
		{

			Enemy = World->SpawnActor<AEnemyPawn>(EnemyClass, SpawnTransform, SpawnParameters);
			
		}
	}
	//Wake the enemy up and place them at the spawn point
	if (Enemy)
	{
		//Set it to the spawn point location
		Enemy -> SetActorTransform(SpawnTransform);
		Enemy -> Activate();
	}

	return Enemy;
}

void UEnemyPoolSubsystem::ReturnEnemy(AEnemyPawn* EnemyToReturn)
{
	if (!EnemyToReturn) return;

	//Check if the enemy is fully dead
	if (EnemyToReturn->Deactivated())
	{
		// Store the enemy back in the correct array based on its class
		TSubclassOf<AEnemyPawn> ClassType = EnemyToReturn->GetClass();
        
		PoolMap.FindOrAdd(ClassType).InactiveEnemies.Push(EnemyToReturn);
	}
	
	
}
