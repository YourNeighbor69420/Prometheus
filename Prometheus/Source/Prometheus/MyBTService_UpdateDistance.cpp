// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBTService_UpdateDistance.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UMyBTService_UpdateDistance::UMyBTService_UpdateDistance()
{
	NodeName = "Update Distance to Target";

	bNotifyBecomeRelevant = false;
}

void UMyBTService_UpdateDistance::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIController = OwnerComp.GetAIOwner();
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (!AIController || !Blackboard) return;

	APawn* AIPawn = AIController->GetPawn();
	if (!AIPawn) return;

	UObject* TargetObject = Blackboard->GetValueAsObject(TargetActorKey.SelectedKeyName);
	AActor* TargetActor = Cast<AActor>(TargetObject);

	if (TargetActor)
	{
		float Distance = FVector::Dist(AIPawn->GetActorLocation(), TargetActor->GetActorLocation());

		Blackboard->SetValueAsFloat(GetSelectedBlackboardKey(), Distance);
	}
	
}
