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

	//Called by timer to end attack
	void FinishTaskAttack(UBehaviorTreeComponent* OwnerComp);

	// The specific blueprint of the bullet/fireball to spawn
	UPROPERTY(EditAnywhere, Category = Combat)
	TSubclassOf<AActor> ProjectileClass;

	//How long between attacks
	UPROPERTY(EditAnywhere, Category = Combat)
	float AttackCooldown = 2.0f;

	//Timer used to track cooldown
	FTimerHandle AttackTimerHandle;
};
