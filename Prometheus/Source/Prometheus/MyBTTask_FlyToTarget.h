// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "MyBTTask_FlyToTarget.generated.h"

/**
 * 
 */
UCLASS()
class PROMETHEUS_API UMyBTTask_FlyToTarget : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UMyBTTask_FlyToTarget();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	// How close the enemy needs to be to the target before stopping
	UPROPERTY(EditAnywhere, Category = "Flight")
	float AcceptableRadius = 100.f;
};
