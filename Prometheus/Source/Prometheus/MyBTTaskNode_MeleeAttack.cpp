// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBTTaskNode_MeleeAttack.h"
#include "EnemyAIController.h"
#include "EnemyPawn.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Pawn.h"
#include "TimerManager.h"

UMyBTTaskNode_MeleeAttack::UMyBTTaskNode_MeleeAttack()
{
	NodeName = "Melee Attack";
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UMyBTTaskNode_MeleeAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	
	AAIController* AIController = OwnerComp.GetAIOwner();
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (!AIController || !Blackboard) return EBTNodeResult::Failed;

	AEnemyPawn* AIPawn = Cast<AEnemyPawn>(AIController->GetPawn());
	if (!AIPawn) return EBTNodeResult::Failed;
	
	UObject* TargetObject = Blackboard->GetValueAsObject(GetSelectedBlackboardKey());
	AActor* TargetActor = Cast<AActor>(TargetObject);
	if (!TargetActor) return EBTNodeResult::Failed;

	AIPawn->PerformAttack(TargetActor);
	
	AIPawn->GetWorldTimerManager().SetTimer(AttackTimerHandle, FTimerDelegate::CreateUObject(this, &UMyBTTaskNode_MeleeAttack::FinishAttackTask, &OwnerComp), AttackDuration, false);

	return EBTNodeResult::InProgress;
}

void UMyBTTaskNode_MeleeAttack::FinishAttackTask(UBehaviorTreeComponent* OwnerComp)
{
	if (OwnerComp)
	{
		FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
	}
}
