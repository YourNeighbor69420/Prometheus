// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBTTaskNode_MeleeAttack.h"
#include "EnemyAIController.h"
#include "GameFramework/Pawn.h"
#include "TimerManager.h"

UMyBTTaskNode_MeleeAttack::UMyBTTask_MeleeAttack()
{
	NodeName = "Melee Attack";
}

EBTNodeResult::Type UMyBTTaskNode_MeleeAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	APawn* AIPawn = AIController->GetPawn();
	if (!AIPawn) return EBTNodeResult::Failed;

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Attacking the player");

	AIPawn->GetWorldTimerManager().SetTimer(AttackTimerHandle, FTimerDelegate::CreateUObject(this, &UMyBTTaskNode_MeleeAttack::FinishAttackTask, OwnerComp), AttackDuration, false);

	return EBTNodeResult::InProgress;
}

void UMyBTTaskNode_MeleeAttack::FinishAttackTask(UBehaviorTreeComponent* OwnerComp)
{
	if (OwnerComp)
	{
		FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
	}
}
