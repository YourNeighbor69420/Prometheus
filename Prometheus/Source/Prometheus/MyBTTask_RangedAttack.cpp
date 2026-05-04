// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBTTask_RangedAttack.h"
#include "EnemyAIController.h"
#include "TimerManager.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Pawn.h"

UMyBTTask_RangedAttack::UMyBTTask_RangedAttack()
{
	//Node name
	NodeName = "Ranged Attack";
	//Create instance to prevent timer overlap
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UMyBTTask_RangedAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	//Grab required AI, Pawn, and Blackboard references
	AAIController* AIController = OwnerComp.GetAIOwner();
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (!AIController || !Blackboard) return EBTNodeResult::Failed;

	APawn* AIPawn = AIController->GetPawn();
	if (!AIPawn) return EBTNodeResult::Failed;

	//Look up the Target Actor
	UObject* TargetObject = Blackboard->GetValueAsObject(GetSelectedBlackboardKey());
	AActor* TargetActor = Cast<AActor>(TargetObject);
	if (!TargetActor) return EBTNodeResult::Failed;

	//Safety check to ensure a projectile was selected in the Editor
	if (!ProjectileClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("Forgot to assign projectile class"));
		return EBTNodeResult::Failed;
	}

	//Calculate spawn position
	FVector SpawnLocation = AIPawn->GetActorLocation() + (AIPawn->GetActorForwardVector() * 300); 
	FVector Direction = TargetActor->GetActorLocation() - SpawnLocation;
	FRotator SpawnRotation = Direction.Rotation();

	//Spawn the projectile into the world
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = AIPawn;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnLocation, SpawnRotation, SpawnParameters);

	//Attack cooldown to prevent machine gun
	AIPawn->GetWorldTimerManager().SetTimer(AttackTimerHandle, FTimerDelegate::CreateUObject(this, &UMyBTTask_RangedAttack::FinishTaskAttack, &OwnerComp), AttackCooldown, false);
	//Keep the task active while the timer is running
	return EBTNodeResult::InProgress;
}

void UMyBTTask_RangedAttack::FinishTaskAttack(UBehaviorTreeComponent* OwnerComp)
{
	if (OwnerComp)
	{
		// Tell the Behavior Tree the attack is complete so it can move to the next node
		FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
	}
}
