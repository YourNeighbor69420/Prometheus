// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "EnemyPawn.h"

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	AEnemyPawn* PossessedEnemy = Cast<AEnemyPawn>(InPawn);
	if (PossessedEnemy)
	{
		UBehaviorTree* TreeToRun = PossessedEnemy->GetBehaviorTree();
		if (TreeToRun)
		{
			RunBehaviorTree(TreeToRun);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("AI Possessed but no tree selected"))
		}
	}
}
