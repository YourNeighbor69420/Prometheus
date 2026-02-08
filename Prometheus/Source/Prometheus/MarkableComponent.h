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
	virtual void OnDashHit_Implementation() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Visuals")
	UWidgetComponent* MarkWidget;

	UPROPERTY(EditAnywhere, Category = "Visuals")
	float SpinSpeed = 90.f;
};
