// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MarkingInterface.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"

#include "MarkableComponent.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;

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

	//Interface implentations
	virtual void OnMarked_Implementation() override;
	virtual void OnUnMarked_Implementation() override;
	virtual void DealDamage_Implementation(float damage) override;
	
	//Getters for combat math
	virtual float GetHealth_Implementation() override;
	virtual float GetMaxHealth_Implementation() override;
	virtual float GetSpeedReward_Implementation() override;
	virtual float GetExecuteSpeedReward_Implementation() override;
	virtual float GetSpeedPenalty_Implementation() override;
	virtual float GetDamageThreshold_Implementation() override;
	virtual void ResetHealth_Implementation() override;

	//Toggles niagara particles
	void SetExecutableEffectActive(bool bIsEffectActive);

	//Set collision on or off
	virtual void Deactivate() override;
	void Activate();

	//Reference for the mark widget
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Visuals")
	UWidgetComponent* MarkWidget;

	//How fast the widget spins
	UPROPERTY(EditAnywhere, Category = "Visuals")
	float SpinSpeed = 90.f;

	float Health;
	
	UPROPERTY(EditAnywhere, Category = "Stats")
	float MaxHealth = 100;

	//Speed reward for dealing damage
	UPROPERTY(EditAnywhere, Category = "Stats")
	float SpeedReward = 1.3;
	//Speed reward for dealing executing
	UPROPERTY(EditAnywhere, Category = "Stats")
	float ExecuteSpeedReward = 1.6;
	//Speed penalty for not being fast enough and doing damage
	UPROPERTY(EditAnywhere, Category = "Stats")
	float SpeedPenalty = 0.67;
	//How much damage you have to do in order to not be penalised
	UPROPERTY(EditAnywhere, Category = "Stats")
	float DamageThreshold;

	//References for the effect that shows if teh enemy can be executed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visuals")
	UNiagaraSystem* ExecutableEffectSystem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visuals")
	UNiagaraComponent* ExecutableEffectComponent;

	
	
};
