// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "MyBTTaskNode_MeleeAttack.generated.h"

/**
 * 
 */
UCLASS()
class PROMETHEUS_API UMyBTTaskNode_MeleeAttack : public UBTTaskNode
{
	GENERATED_BODY()
	

public:
	UMyBTTask_MeleeAttack();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	//What fires once the attack animation is done
	void FinishAttackTask(UBehaviorTreeComponent* OwnerComp);

	//How long should the behaviour tree wait 
	UPROPERTY(EditAnywhere, category = "Combat")
	float AttackDuration = 1.5f;

	FTimerHandle AttackTimerHandle;

};
