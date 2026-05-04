// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBTService_UpdateDistance.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UMyBTService_UpdateDistance::UMyBTService_UpdateDistance()
{
	//Sets node name in editor
	NodeName = "Update Distance to Target";
	//Prevent unnecessary triggers
	bNotifyBecomeRelevant = false;
}

void UMyBTService_UpdateDistance::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	//Grab the AI Controller, Blackboard, and the physical Enemy Pawn
	AAIController* AIController = OwnerComp.GetAIOwner();
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (!AIController || !Blackboard) return;

	APawn* AIPawn = AIController->GetPawn();
	if (!AIPawn) return;

	//Look up who the AI is currently targeting
	UObject* TargetObject = Blackboard->GetValueAsObject(TargetActorKey.SelectedKeyName);
	AActor* TargetActor = Cast<AActor>(TargetObject);

	if (TargetActor)
	{
		//Calculate the exact distance between the AI and the target
		float Distance = FVector::Dist(AIPawn->GetActorLocation(), TargetActor->GetActorLocation());
		//Save that distance back to the Blackboard
		Blackboard->SetValueAsFloat(GetSelectedBlackboardKey(), Distance);
	}
	
}
