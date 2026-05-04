// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PrometheusCharacter.h"
#include "GameFramework/Pawn.h"
#include "BehaviorTree/BehaviorTree.h"
#include "EnemyPawn.generated.h"

class AArenaManager;

UCLASS()
class PROMETHEUS_API AEnemyPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AEnemyPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Reference to its arena
	UPROPERTY()
	AArenaManager* OwningArena;

	//Reference to what effect it has
	UPROPERTY()
	UMaterialInstanceDynamic* MaterialInstance;

	//Reference to the player
	UPROPERTY()
	APrometheusCharacter* PlayerRef;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//Attack the player
	void PerformAttack(AActor* TargetActor);
	//Check if attack landed
	void ExecuteDamageCheck();

	//Reference to the current player 
	UPROPERTY()
	AActor* CurrentTargetActor;

	//Attack time handle
	FTimerHandle AttackTimerHandle;

	//Reference to the it's AI behaviour
	UPROPERTY(EditAnywhere, Category = "AI")
	UBehaviorTree* EnemyBehaviorTree;

	//How long it takes to attack
	UPROPERTY(EditAnywhere, Category = "Attack")
	float AttackSpeed = 1.f;

	//Distance that the attack can be executed from
	UPROPERTY(EditAnywhere, Category = "Attack")
	float AttackReach = 300.f;

	//How much damage the attack does
	UPROPERTY(EditAnywhere, Category = "Attack")
	float AttackDamage= 1.f;

	//Overall enemy health via number of components
	int AliveComponents = 1;

	//It's max number of components
	UPROPERTY(EditAnywhere, Category = "Health")
	int MaxAliveComponents = 1;

	//A list of all markable components on enemy
	TArray<UMarkableComponent*> AllMarkers;

	//Returns enemy's behaviour tree
	UBehaviorTree* GetBehaviorTree();

	//How fast the enemy turns
	UPROPERTY(EditAnywhere, Category = "Movement")
	float TurnSpeed = 2.0f;

	//Called when component breaks, returns true if fully dead
	UFUNCTION(BlueprintCallable, Category = "Pooling")
	bool Deactivated();

	//Deactivate enemy by shutting off all game systems
	UFUNCTION(BlueprintCallable, Category = "Pooling")
	void FullyDeactivate();

	//Reactivate enemy by turning on all game systems
	UFUNCTION(BlueprintCallable, Category = "Pooling")
	virtual void Activate();
	
	bool bIsAlive = true;

	//Set the arena this enemy belongs to 
	void SetOwningArena(AArenaManager* Arena) { OwningArena = Arena;}

	//Blueprint event to signal component health reset
	UFUNCTION(BlueprintImplementableEvent, Category = "Pooling")
	void OnReActivate();
};
