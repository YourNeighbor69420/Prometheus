// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBTService_FindPlayer.h"
#include "PrometheusCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

UMyBTService_FindPlayer::UMyBTService_FindPlayer()
{
	NodeName = "Update Player Target";
}

void UMyBTService_FindPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();

	if (BlackboardComponent && PlayerCharacter)
	{
		BlackboardComponent->SetValueAsObject(GetSelectedBlackboardKey(), PlayerCharacter);
	}
}
