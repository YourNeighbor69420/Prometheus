// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyPawn.h"
#include "Engine/TargetPoint.h"
#include "GameFramework/Actor.h"
#include "ArenaManager.generated.h"

//Event dispatcher declaration for when an arena finishes
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnArenaFinished);

//A data container for the enemy parameters to be set in editor
USTRUCT(BlueprintType)
struct FSpawnInstructions
{
	GENERATED_BODY()
	//The enemy class to spawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Spawning")
	TSubclassOf<AEnemyPawn> EnemyClassToSpawn;
	//Where to spawn the enemy
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	ATargetPoint* SpawnLocation;
};

//A data container for the arena phases parameters to be set in editor
USTRUCT(BlueprintType)
struct FArenaPhase
{
	GENERATED_BODY()
	// A list of every enemy that needs to be spawned during this wave
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	TArray<FSpawnInstructions> SpawnInstructions;
	//The pause time before the next phase starts
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	float DelayBeforeStart = 2.0f;
};
UCLASS()
class PROMETHEUS_API AArenaManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AArenaManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//The list of all phases/waves in the arena
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phases")
	TArray<FArenaPhase> ArenaPhases;
	
	//Which wave are we on
	int32 CurrentPhaseIndex = 0;
	//How many enemies are in the current room
	int32 ActiveEnemiesInRoom = 0;

	//The collision box to activate the arena
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	class UBoxComponent* TriggerBox;

	//The function that fires when the player overlaps it
	UFUNCTION()
	void OnPlayerEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	//Begins the arena by reading phase data and spawning enemies
	void StartPhase();
	// Fires the victory delegate when all phases are empty
	void EndArena();
	//List of every active enemy in the room
	UPROPERTY()
	TArray<AEnemyPawn*> ActiveEnemiesArray;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Enemies call this function on themselves right before they die or go back to the pool
	UFUNCTION(BlueprintCallable)
	void ReportEnemyDeath();
	//Reset switch called by the player on respawn
	UFUNCTION(BlueprintCallable)
	void ResetArena();
	//Lets the game know if it's currently resetting 
	bool bIsResetting = false;
	//Timer for the arena delay between phases
	FTimerHandle PhaseTimerHandle;
	//The event dispatcher for when the arena finishes
	UPROPERTY(BlueprintAssignable, Category = "Phases")
	FOnArenaFinished OnArenaFinished;

};
