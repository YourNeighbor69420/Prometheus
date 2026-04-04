// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyPawn.h"

#include "AIController.h"
#include "ArenaManager.h"
#include "PlayerDamageInterface.h"
#include "PrometheusCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AEnemyPawn::AEnemyPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
}

// Called when the game starts or when spawned
void AEnemyPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector CurrentVelocity = GetVelocity();

	if (CurrentVelocity.SizeSquared() > 10.f)
	{
		FRotator CurrentRotation = GetActorRotation();
		FRotator TargetRotation = CurrentVelocity.Rotation();

		FRotator SmoothRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, TurnSpeed);

		SetActorRotation(SmoothRotation);
	}

}

// Called to bind functionality to input
void AEnemyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemyPawn::PerformAttack(AActor* TargetActor)
{
	CurrentTargetActor = TargetActor;
	GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &AEnemyPawn::ExecuteDamageCheck, AttackSpeed, false);
}

void AEnemyPawn::ExecuteDamageCheck()
{
	if (CurrentTargetActor)
	{

		float Distance = FVector::Dist(GetActorLocation(), CurrentTargetActor->GetActorLocation());

		if (Distance <= AttackReach)
		{
			if (CurrentTargetActor->Implements<UPlayerDamageInterface>())
			{
				IPlayerDamageInterface::Execute_ApplyPlayerDamage(CurrentTargetActor, AttackDamage);
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, TEXT("enemy hit the player"));
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Player dodged attack"));
		}
	}
	CurrentTargetActor = nullptr;
}


UBehaviorTree* AEnemyPawn::GetBehaviorTree()
{
	return EnemyBehaviorTree;
}

bool AEnemyPawn::Deactivated()
{
	AliveComponents--;
	
	if (AliveComponents == 0)
	{
		SetActorTickEnabled(false);
        	
        	SetActorHiddenInGame(true);
        
        	SetActorEnableCollision(false);
        
        	AAIController* AiController = Cast<AAIController>(GetController());
        	if (AiController)
        	{
        		AiController->StopMovement();
        		UBrainComponent* EnemyBrain = AiController->GetBrainComponent();
        		if (EnemyBrain)
        		{
        			EnemyBrain->StopLogic("Deactivated");
        			bIsAlive = false;
        			AliveComponents = MaxAliveComponents;
        		}
        	}
        
        	if (OwningArena)
        	{
        		OwningArena->ReportEnemyDeath();
        
        		OwningArena = nullptr;
        	}

		return true;
	}
	else
	{
		
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, TEXT("Killed component"));
		return false;
	}
	
	
}

void AEnemyPawn::Activate()
{
	SetActorTickEnabled(true);
	
	SetActorHiddenInGame(false);

	SetActorEnableCollision(true);
	
	AAIController* AiController = Cast<AAIController>(GetController());
	if (AiController)
	{
		UBrainComponent* EnemyBrain = AiController->GetBrainComponent();
		if (EnemyBrain)
		{
			EnemyBrain->StartLogic();
			bIsAlive = true;
		}
	}
}

