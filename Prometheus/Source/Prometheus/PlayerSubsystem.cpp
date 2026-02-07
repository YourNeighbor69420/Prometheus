// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerSubsystem.h"
#include "MarkableComponent.h"

void UPlayerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, "UPlayerSubsystem::Initialize");
	
}

void UPlayerSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UPlayerSubsystem::SetMarkedTarget(UMarkableComponent* NewMarkedTarget)
{
	UE_LOG(LogTemp, Error, TEXT("Subsystem Address: %p"), this);
	if (NewMarkedTarget)
	{
		CurrentMarkedTarget = NewMarkedTarget;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, "UPlayerSubsystem::SetMarkedTarget");
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, "BADDDDD");

	}

}

