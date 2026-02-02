// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerSubsystem.h"

void UPlayerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, "UPlayerSubsystem::Initialize");
}

void UPlayerSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UPlayerSubsystem::Mark()
{
	
}
