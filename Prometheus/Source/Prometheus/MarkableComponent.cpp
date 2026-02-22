// Fill out your copyright notice in the Description page of Project Settings.


#include "MarkableComponent.h"

#include "Components/WidgetComponent.h"

// Sets default values for this component's properties
UMarkableComponent::UMarkableComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	MarkWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("MarkWidget"));
	MarkWidget->SetupAttachment(this);

	MarkWidget->SetVisibility(false);
	MarkWidget->SetCachedMaxDrawDistance(true);
	MarkWidget->SetWidgetSpace(EWidgetSpace::Screen);
	
	

	// ...
}


// Called when the game starts
void UMarkableComponent::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;
	DamageThreshold = Health / 2;
	// ...
	
}


// Called every frame
void UMarkableComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UMarkableComponent::OnMarked_Implementation()
{
	IMarkingInterface::OnMarked_Implementation();

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Emerald, "Marked Component");
	MarkWidget->SetVisibility(true);
}

void UMarkableComponent::OnUnMarked_Implementation()
{
	IMarkingInterface::OnUnMarked_Implementation();
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Emerald, "UnMarked Component");
	MarkWidget->SetVisibility(false);


}

void UMarkableComponent::DealDamage_Implementation(float damage)
{
	IMarkingInterface::DealDamage_Implementation(damage);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("damage dealt: %f"), damage));

	Health -= damage;
	Health = FMath::Clamp(Health, 0, MaxHealth);
	
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("enemy health: %f / %f"), Health, MaxHealth));

	if (Health == 0.f)
	{
		GetOwner()->Destroy();
	}
}

float UMarkableComponent::GetHealth_Implementation()
{
	IMarkingInterface::GetHealth_Implementation();

	return Health;
}

float UMarkableComponent::GetMaxHealth_Implementation()
{
	IMarkingInterface::GetMaxHealth_Implementation();

	return MaxHealth;
}

float UMarkableComponent::GetSpeedReward_Implementation()
{
	IMarkingInterface::GetSpeedReward_Implementation();

	return SpeedReward;
}

float UMarkableComponent::GetExecuteSpeedReward_Implementation()
{
	IMarkingInterface::GetExecuteSpeedReward_Implementation();

	return ExecuteSpeedReward;
}

float UMarkableComponent::GetSpeedPenalty_Implementation()
{
	IMarkingInterface::GetSpeedPenalty_Implementation();

	return SpeedPenalty;
}

float UMarkableComponent::GetDamageThreshold_Implementation()
{
	IMarkingInterface::GetDamageThreshold_Implementation();

	return DamageThreshold;
}


