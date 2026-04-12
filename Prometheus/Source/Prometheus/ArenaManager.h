// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyPawn.h"
#include "Engine/TargetPoint.h"
#include "GameFramework/Actor.h"
#include "ArenaManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnArenaFinished);

USTRUCT(BlueprintType)
struct FSpawnInstructions
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Spawning")
	TSubclassOf<AEnemyPawn> EnemyClassToSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	ATargetPoint* SpawnLocation;
};

USTRUCT(BlueprintType)
struct FArenaPhase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	TArray<FSpawnInstructions> SpawnInstructions;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phases")
	TArray<FArenaPhase> ArenaPhases;

	int32 CurrentPhaseIndex = 0;

	int32 ActiveEnemiesInRoom = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	class UBoxComponent* TriggerBox;

	UFUNCTION()
	void OnPlayerEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	void StartPhase();

	void EndArena();
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void ReportEnemyDeath();

	

	FTimerHandle PhaseTimerHandle;

	UPROPERTY(BlueprintAssignable, Category = "Phases")
	FOnArenaFinished OnArenaFinished;

};
