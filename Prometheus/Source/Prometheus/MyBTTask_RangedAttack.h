// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "MyBTTask_RangedAttack.generated.h"

/**
 * 
 */
UCLASS()
class PROMETHEUS_API UMyBTTask_RangedAttack : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UMyBTTask_RangedAttack();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	void FinishTaskAttack(UBehaviorTreeComponent* OwnerComp);

	UPROPERTY(EditAnywhere, Category = Combat)
	TSubclassOf<AActor> ProjectileClass;

	UPROPERTY(EditAnywhere, Category = Combat)
	float AttackCooldown = 2.0f;

	FTimerHandle AttackTimerHandle;
};
