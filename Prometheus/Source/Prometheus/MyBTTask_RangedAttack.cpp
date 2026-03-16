// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBTTask_RangedAttack.h"
#include "EnemyAIController.h"
#include "TimerManager.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Pawn.h"

UMyBTTask_RangedAttack::UMyBTTask_RangedAttack()
{
	NodeName = "Ranged Attack";
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UMyBTTask_RangedAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{

	AAIController* AIController = OwnerComp.GetAIOwner();
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (!AIController || !Blackboard) return EBTNodeResult::Failed;

	APawn* AIPawn = AIController->GetPawn();
	if (!AIPawn) return EBTNodeResult::Failed;

	UObject* TargetObject = Blackboard->GetValueAsObject(GetSelectedBlackboardKey());
	AActor* TargetActor = Cast<AActor>(TargetObject);
	if (!TargetActor) return EBTNodeResult::Failed;

	if (!ProjectileClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("Forgot to assign projectile class"));
		return EBTNodeResult::Failed;
	}

	FVector SpawnLocation = AIPawn->GetActorLocation() + (AIPawn->GetActorForwardVector() * 300); 

	FVector Direction = TargetActor->GetActorLocation() - SpawnLocation;
	FRotator SpawnRotation = Direction.Rotation();

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = AIPawn;

	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnLocation, SpawnRotation, SpawnParameters);

	AIPawn->GetWorldTimerManager().SetTimer(AttackTimerHandle, FTimerDelegate::CreateUObject(this, &UMyBTTask_RangedAttack::FinishTaskAttack, &OwnerComp), AttackCooldown, false);

	return EBTNodeResult::InProgress;
}

void UMyBTTask_RangedAttack::FinishTaskAttack(UBehaviorTreeComponent* OwnerComp)
{
	if (OwnerComp)
	{
		FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
	}
}
