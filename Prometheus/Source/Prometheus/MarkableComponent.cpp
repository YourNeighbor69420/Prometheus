// Fill out your copyright notice in the Description page of Project Settings.


#include "MarkableComponent.h"

// Sets default values for this component's properties
UMarkableComponent::UMarkableComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UMarkableComponent::BeginPlay()
{
	Super::BeginPlay();

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

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Emerald, "Marked Component");
}

void UMarkableComponent::OnDashHit_Implementation()
{
	IMarkingInterface::OnDashHit_Implementation();

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Hit Enemy");
}

