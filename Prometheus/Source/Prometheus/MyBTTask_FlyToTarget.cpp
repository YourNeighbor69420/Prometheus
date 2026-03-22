// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBTTask_FlyToTarget.h"
#include "EnemyAIController.h"
#include "EnemyPawn.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Pawn.h"

UMyBTTask_FlyToTarget::UMyBTTask_FlyToTarget()
{
	NodeName = "Fly To Target";

	//keep tick active
	bNotifyTick = true;
}

EBTNodeResult::Type UMyBTTask_FlyToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	//Let the code know to continue running on tick
	return EBTNodeResult::InProgress;
}

void UMyBTTask_FlyToTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	//Get the controller, if not return nothing
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return;

	//Get the controllers pawn and blackboard
	APawn* Pawn = AIController->GetPawn();
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();

	//If either isn't available, prevent crash
	if (!BlackboardComponent || !Pawn)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	//Get the selected key in editor and cast it as an actor to be found in world 
	UObject* TargetObject = BlackboardComponent->GetValueAsObject(GetSelectedBlackboardKey());
	AActor* TargetActor = Cast<AActor>(TargetObject);
	
	FVector PawnLoc = Pawn->GetActorLocation();
	if (TargetActor)
	{
		FVector TargetLoc = TargetActor->GetActorLocation();
		float Distance = FVector::Dist(PawnLoc, TargetLoc);

		if (Distance <= AcceptableRadius)
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			return;
		}

		FVector Direction = (TargetLoc - PawnLoc).GetSafeNormal();
		Pawn->AddMovementInput(Direction,1.f);
	}
}
