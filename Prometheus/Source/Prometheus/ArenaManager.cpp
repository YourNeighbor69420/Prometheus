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

	if (TriggerBox)
	{
		TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AArenaManager::OnPlayerEnter);
	}
	
}

void AArenaManager::OnPlayerEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("WAVE CHECK -> CurrentIndex: %d | TotalPhases: %d"), CurrentPhaseIndex, ArenaPhases.Num()));

	APrometheusCharacter*  Player = Cast<APrometheusCharacter>(OtherActor);
	if (Player)
	{
		TriggerBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		Player->CurrentArenaManager = this;

		StartPhase();
	}
}

void AArenaManager::StartPhase()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("WAVE CHECK -> CurrentIndex: %d | TotalPhases: %d"), CurrentPhaseIndex, ArenaPhases.Num()));
	
	
	FArenaPhase& CurrentPhase = ArenaPhases[CurrentPhaseIndex];

	UEnemyPoolSubsystem* EnemyPoolSubsystem = GetWorld()->GetSubsystem<UEnemyPoolSubsystem>();

	for (const FSpawnInstructions& Instructions : CurrentPhase.SpawnInstructions)
	{
		if (Instructions.EnemyClassToSpawn && Instructions.SpawnLocation)
		{
			AEnemyPawn* SpawnedEnemy = EnemyPoolSubsystem->RequestEnemy(Instructions.EnemyClassToSpawn, Instructions.SpawnLocation->GetActorTransform());

			if (SpawnedEnemy)
			{
				SpawnedEnemy->SetOwningArena(this);
				ActiveEnemiesInRoom++;
				
				ActiveEnemiesArray.Add(SpawnedEnemy);
			}
		}
	}
}

void AArenaManager::EndArena()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "arena dopne");
	OnArenaFinished.Broadcast();
}

// Called every frame
void AArenaManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AArenaManager::ReportEnemyDeath()
{
	if (bPlayerAlive)
	{
		return;
	}
	ActiveEnemiesInRoom--;

	if (ActiveEnemiesInRoom <= 0)
	{
		ActiveEnemiesInRoom = 0;

		CurrentPhaseIndex++;

		if (CurrentPhaseIndex < ArenaPhases.Num())
		{
			float Delay = ArenaPhases[CurrentPhaseIndex].DelayBeforeStart;

			GetWorld()->GetTimerManager().SetTimer(PhaseTimerHandle, this, &AArenaManager::StartPhase, Delay, false);
		}
		else if (ArenaPhases.IsEmpty() || CurrentPhaseIndex >= ArenaPhases.Num())
		{
			//Room Cleared
			EndArena();
		}
	}
}

void AArenaManager::ResetArena()
{
	bPlayerAlive = true;
	GetWorldTimerManager().ClearTimer(PhaseTimerHandle);

	for (AEnemyPawn* Enemy : ActiveEnemiesArray )
	{
		
		Enemy->FullyDeactivate();
	}

	ActiveEnemiesArray.Empty();

	CurrentPhaseIndex = 0;
	ActiveEnemiesInRoom = 0;

	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	bPlayerAlive = false;
	
}

