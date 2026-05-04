// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "EnemyPawn.h"

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	//Ensure the pawn we possessed is actually one of our custom enemies
	AEnemyPawn* PossessedEnemy = Cast<AEnemyPawn>(InPawn);
	if (PossessedEnemy)
	{
		//Ask the enemy which Behavior Tree it wants to use
		UBehaviorTree* TreeToRun = PossessedEnemy->GetBehaviorTree();
		if (TreeToRun)
		{
			//Start AI logic
			RunBehaviorTree(TreeToRun);
		}
		else
		{
			//Warn developer
			UE_LOG(LogTemp, Error, TEXT("AI Possessed but no tree selected"))
		}
	}
}
