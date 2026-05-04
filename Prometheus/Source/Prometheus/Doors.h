// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ArenaManager.h"
#include "GameFramework/Actor.h"
#include "Doors.generated.h"

UCLASS()
class PROMETHEUS_API ADoors : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADoors();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Create a static mesh for the door
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* DoorsMesh;

	//Removes the door 
	UFUNCTION()
	void UnlockDoor();

	//What arena manages the door
	UPROPERTY(EditAnywhere, category = "Doors")
	AArenaManager* DoorsManager;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
