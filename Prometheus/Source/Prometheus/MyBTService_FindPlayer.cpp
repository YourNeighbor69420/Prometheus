// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBTService_FindPlayer.h"
#include "PrometheusCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

UMyBTService_FindPlayer::UMyBTService_FindPlayer()
{
	//Sets node name in editor
	NodeName = "Update Player Target";
}

void UMyBTService_FindPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	//Find the player in the world
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	//Get AI brain
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();

	//Save the player into the AI's memory so other nodes can track or attack them
	if (BlackboardComponent && PlayerCharacter)
	{
		BlackboardComponent->SetValueAsObject(GetSelectedBlackboardKey(), PlayerCharacter);
	}
}
