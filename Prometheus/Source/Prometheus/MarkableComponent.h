// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MarkingInterface.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"

#include "MarkableComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROMETHEUS_API UMarkableComponent : public USphereComponent, public IMarkingInterface
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMarkableComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void OnMarked_Implementation() override;
	virtual void OnUnMarked_Implementation() override;
	virtual void DealDamage_Implementation(float damage) override;
	virtual float GetHealth_Implementation() override;
	virtual float GetMaxHealth_Implementation() override;
	virtual float GetSpeedReward_Implementation() override;
	virtual float GetExecuteSpeedReward_Implementation() override;
	virtual float GetSpeedPenalty_Implementation() override;
	virtual float GetDamageThreshold_Implementation() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Visuals")
	UWidgetComponent* MarkWidget;

	UPROPERTY(EditAnywhere, Category = "Visuals")
	float SpinSpeed = 90.f;

	float Health;
	
	UPROPERTY(EditAnywhere, Category = "Stats")
	float MaxHealth = 100;

	UPROPERTY(EditAnywhere, Category = "Stats")
	float SpeedReward = 1.2;
	
	UPROPERTY(EditAnywhere, Category = "Stats")
	float ExecuteSpeedReward = 1.4;

	UPROPERTY(EditAnywhere, Category = "Stats")
	float SpeedPenalty = 0.67;
	
	UPROPERTY(EditAnywhere, Category = "Stats")
	float DamageThreshold;
	
	
};
