// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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

	UPROPERTY()
	AArenaManager* OwningArena;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	
	void PerformAttack(AActor* TargetActor);

	void ExecuteDamageCheck();

	UPROPERTY()
	AActor* CurrentTargetActor;

	FTimerHandle AttackTimerHandle;

	UPROPERTY(EditAnywhere, Category = "AI")
	UBehaviorTree* EnemyBehaviorTree;

	UPROPERTY(EditAnywhere, Category = "Attack")
	float AttackSpeed = 1.f;
	
	UPROPERTY(EditAnywhere, Category = "Attack")
	float AttackReach = 300.f;

	UPROPERTY(EditAnywhere, Category = "Attack")
	float AttackDamage= 1.f;

	UPROPERTY(EditAnywhere, Category = "Health")
	int AliveComponents = 1;
	
	UBehaviorTree* GetBehaviorTree();

	UPROPERTY(EditAnywhere, Category = "Movement")
	float TurnSpeed = 2.0f;

	UFUNCTION(BlueprintCallable, Category = "Pooling")
	bool Deactivated();
	
	UFUNCTION(BlueprintCallable, Category = "Pooling")
	virtual void Activate();

	bool bIsAlive = 0;

	void SetOwningArena(AArenaManager* Arena) { OwningArena = Arena;}
};
