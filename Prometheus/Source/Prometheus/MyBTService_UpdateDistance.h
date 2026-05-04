// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "MyBTService_UpdateDistance.generated.h"

/**
 * 
 */
UCLASS()
class PROMETHEUS_API UMyBTService_UpdateDistance : public UBTService_BlackboardBase
{
	GENERATED_BODY()

public:
	UMyBTService_UpdateDistance();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	//Reference to who the ai is targeting
	UPROPERTY(EditAnywhere, Category = Behavior)
	FBlackboardKeySelector TargetActorKey;
};
