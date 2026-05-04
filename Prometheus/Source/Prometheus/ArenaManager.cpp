// Fill out your copyright notice in the Description page of Project Settings.


#include "ArenaManager.h"

#include "EnemyPoolSubsystem.h"
#include "PrometheusCharacter.h"
#include "Components/BoxComponent.h"

// Sets default values
AArenaManager::AArenaManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	RootComponent = TriggerBox;
}

// Called when the game starts or when spawned
void AArenaManager::BeginPlay()
{
	Super::BeginPlay();

	//If triggerbox is valid and overlapped, call OnPlayerEnter
	if (TriggerBox)
	{
		TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AArenaManager::OnPlayerEnter);
	}
	
}

void AArenaManager::OnPlayerEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("WAVE CHECK -> CurrentIndex: %d | TotalPhases: %d"), CurrentPhaseIndex, ArenaPhases.Num()));

	//If the player overlaps
	APrometheusCharacter*  Player = Cast<APrometheusCharacter>(OtherActor);
	if (Player)
	{
		//Turn off triggerbox to avoid multiple triggers
		TriggerBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		//Set current arena to this specific actor
		Player->CurrentArenaManager = this;
		//Start the arena 
		StartPhase();
	}
}

void AArenaManager::StartPhase()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("WAVE CHECK -> CurrentIndex: %d | TotalPhases: %d"), CurrentPhaseIndex, ArenaPhases.Num()));
	
	//Get the current phase we are on
	FArenaPhase& CurrentPhase = ArenaPhases[CurrentPhaseIndex];
	//Grab our object pool
	UEnemyPoolSubsystem* EnemyPoolSubsystem = GetWorld()->GetSubsystem<UEnemyPoolSubsystem>();
	//Loop through our editor set spawn instructions for our current wave
	for (const FSpawnInstructions& Instructions : CurrentPhase.SpawnInstructions)
	{
		//Ensure editor has set parameters
		if (Instructions.EnemyClassToSpawn && Instructions.SpawnLocation)
		{
			//Get an enemy from the object pool and place them at their set spawn point
			AEnemyPawn* SpawnedEnemy = EnemyPoolSubsystem->RequestEnemy(Instructions.EnemyClassToSpawn, Instructions.SpawnLocation->GetActorTransform());

			//If we got an enemy
			if (SpawnedEnemy)
			{
				//Let it know which manager is belongs to
				SpawnedEnemy->SetOwningArena(this);
				//Add to existing number of arena enemies
				ActiveEnemiesInRoom++;
				//Store the enemy in an array for later clean up
				ActiveEnemiesArray.Add(SpawnedEnemy);
			}
		}
	}
}

void AArenaManager::EndArena()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "arena done");
	//Broadcast when arena delegate finishes
	OnArenaFinished.Broadcast();
}

// Called every frame
void AArenaManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AArenaManager::ReportEnemyDeath()
{
	//If we are resetting the arena, don't report the clean up deaths
	if (bIsResetting)
	{
		return;
	}
	//Decrease enemy count in room
	ActiveEnemiesInRoom--;

	//If the room is empty
	if (ActiveEnemiesInRoom <= 0)
	{
		//Reset enemy numbers to empty
		ActiveEnemiesInRoom = 0;

		//Move to next phase
		CurrentPhaseIndex++;

		//Check if there are more phases left
		if (CurrentPhaseIndex < ArenaPhases.Num())
		{
			//Get the pause duration before the next wave starts
			float Delay = ArenaPhases[CurrentPhaseIndex].DelayBeforeStart;

			//Set a timer to start the next phase after the delay
			GetWorld()->GetTimerManager().SetTimer(PhaseTimerHandle, this, &AArenaManager::StartPhase, Delay, false);
		}
		//If there are no phases left, end the arena
		else if (ArenaPhases.IsEmpty() || CurrentPhaseIndex >= ArenaPhases.Num())
		{
			//Room Cleared
			EndArena();
		}
	}
}

void AArenaManager::ResetArena()
{
	//Let the game know its resetting to avoid errors
	bIsResetting = true;
	//Stop any pending waves from starting 
	GetWorldTimerManager().ClearTimer(PhaseTimerHandle);

	//Deactivate any existing enemies 
	for (AEnemyPawn* Enemy : ActiveEnemiesArray )
	{
		Enemy->FullyDeactivate();
	}
	//Clear the current enemies in the cleanup array
	ActiveEnemiesArray.Empty();
	//Reset phases and enemy number
	CurrentPhaseIndex = 0;
	ActiveEnemiesInRoom = 0;
	//Re-enable the collision to let the arena manager start again
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	//Let the game know it's done resetting
	bIsResetting = false;
	
}

