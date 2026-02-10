// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MarkableComponent.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "PlayerSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class PROMETHEUS_API UPlayerSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable)
	void SetMarkedTarget(UMarkableComponent* NewMarkedTarget);

	UFUNCTION(BlueprintCallable)
	UMarkableComponent* GetMarkedTarget() const;

	
	
private:
	TWeakObjectPtr<UMarkableComponent> CurrentMarkedTarget;
};
