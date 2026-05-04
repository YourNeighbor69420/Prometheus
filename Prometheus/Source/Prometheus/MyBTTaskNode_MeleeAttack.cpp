// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBTTaskNode_MeleeAttack.h"
#include "EnemyAIController.h"
#include "EnemyPawn.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Pawn.h"
#include "TimerManager.h"

UMyBTTaskNode_MeleeAttack::UMyBTTaskNode_MeleeAttack()
{
	//Set node name
	NodeName = "Melee Attack";
	//Create instance to prevent timer overlap
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UMyBTTaskNode_MeleeAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	//Grab required AI, Pawn, and Blackboard references
	AAIController* AIController = OwnerComp.GetAIOwner();
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (!AIController || !Blackboard) return EBTNodeResult::Failed;

	AEnemyPawn* AIPawn = Cast<AEnemyPawn>(AIController->GetPawn());
	if (!AIPawn) return EBTNodeResult::Failed;

	//Look up the Target Actor
	UObject* TargetObject = Blackboard->GetValueAsObject(GetSelectedBlackboardKey());
	AActor* TargetActor = Cast<AActor>(TargetObject);
	if (!TargetActor) return EBTNodeResult::Failed;

	//Tell enemy to attack
	AIPawn->PerformAttack(TargetActor);

	//Start a timer to pause the Behavior Tree while the attack happens
	AIPawn->GetWorldTimerManager().SetTimer(AttackTimerHandle, FTimerDelegate::CreateUObject(this, &UMyBTTaskNode_MeleeAttack::FinishAttackTask, &OwnerComp), AttackDuration, false);

	// Keep the node running until the timer finishes
	return EBTNodeResult::InProgress;
}

void UMyBTTaskNode_MeleeAttack::FinishAttackTask(UBehaviorTreeComponent* OwnerComp)
{
	if (OwnerComp)
	{
		// Tell the Behavior Tree the attack is complete so it can move on
		FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
	}
}
