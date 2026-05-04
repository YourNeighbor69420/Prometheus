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
	
	//Get references to the player and all markable body parts on spawn
	PlayerRef = Cast<APrometheusCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	GetComponents<UMarkableComponent>(AllMarkers);
}

// Called every frame
void AEnemyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector CurrentVelocity = GetVelocity();

	//Rotate the enemy to face the direction that the enemy is moving
	if (CurrentVelocity.SizeSquared() > 10.f)
	{
		FRotator CurrentRotation = GetActorRotation();
		FRotator TargetRotation = CurrentVelocity.Rotation();

		//make sure the rotation is smooth
		FRotator SmoothRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, TurnSpeed);
		SetActorRotation(SmoothRotation);
	}

	//Check to see if the player can kill a markable component, if so, show the effect
	if (PlayerRef)
	{
		for (UMarkableComponent* Marker : AllMarkers)
		{
			//Get the components health and see how fast the player is going/how much damage they can do
			float CompHealth = Marker->GetHealth_Implementation();
			float PlayerDamage = PlayerRef->GetDamage();

			//Check if the player can instantly kill the component
			if (PlayerDamage >= CompHealth)
			{
				Marker->SetExecutableEffectActive(true);
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Silver, "Set effect ACTIVE");
			}
			else
			{
				Marker->SetExecutableEffectActive(false);
			}
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Silver, "player ref doesnt work");

	}
	
	

}

// Called to bind functionality to input
void AEnemyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemyPawn::PerformAttack(AActor* TargetActor)
{
	// Starts the attack process and sets a delay based on the enemy's attack speed
	CurrentTargetActor = TargetActor;
	GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &AEnemyPawn::ExecuteDamageCheck, AttackSpeed, false);
}

void AEnemyPawn::ExecuteDamageCheck()
{
	
	if (CurrentTargetActor)
	{
		//Get distance between enemy and player
		float Distance = FVector::Dist(GetActorLocation(), CurrentTargetActor->GetActorLocation());
		
		//When the timer finishes, check to see if the player is close enough to take damage
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
	//Get enemy's behaviour tree
	return EnemyBehaviorTree;
}

bool AEnemyPawn::Deactivated()
{
	//Decrease component amount
	AliveComponents--;

	//Check if there are any remaining components
	if (AliveComponents == 0)
	{
		//If dead, shut down all systems and deactivate the enemy
		FullyDeactivate();
		return true;
	}
	else
	{
		
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, TEXT("Killed component"));
		return false;
	}
	
	
}

void AEnemyPawn::FullyDeactivate()
{

	//Deactivate enemy and shut down systems
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
        			
		}
	}

	//Tell arena it's attached to that it's dead
	if (OwningArena)
	{
		OwningArena->ReportEnemyDeath();
        
		OwningArena = nullptr;
	}
}

void AEnemyPawn::Activate()
{
	// Reset visual components
	if (UMarkableComponent* MarkableComponent = GetComponentByClass<UMarkableComponent>())
	{
		MarkableComponent->SetVisibility(false);
	}

	//Re-enable markers and their collision
	for (UMarkableComponent* Marker : AllMarkers)
	{
		Marker->Activate();
	}
	//Turn back on physic systems and rendering
	SetActorTickEnabled(true);
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);

	
	//Turn AI brain back online
	AAIController* AiController = Cast<AAIController>(GetController());
	if (AiController)
	{
		UBrainComponent* EnemyBrain = AiController->GetBrainComponent();
		if (EnemyBrain)
		{
			EnemyBrain->StartLogic();
			bIsAlive = true;

			AliveComponents = MaxAliveComponents;

			//Call blueprint event
			OnReActivate();
		}
	}
}

