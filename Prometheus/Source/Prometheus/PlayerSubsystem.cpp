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
	//UE_LOG(LogTemp, Error, TEXT("Subsystem Address: %p"), this);

	//If we already have a target, turn off its UI marker before switching
	if (CurrentMarkedTarget.IsValid())
	{
		IMarkingInterface::Execute_OnUnMarked(CurrentMarkedTarget.Get());
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow,"this does work");
	}

	//Assign new target
	CurrentMarkedTarget = NewMarkedTarget;

	//If the new target hit, turn on its UI marker
	if (NewMarkedTarget)
	{
		IMarkingInterface::Execute_OnMarked(NewMarkedTarget);
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, FString::Printf(TEXT("Target Successfully marked: %s"), *NewMarkedTarget->GetName()));
	}
	else
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, "BADDDDD");
	}

}

void UPlayerSubsystem::ClearMarkedTarget()
{
	// Clears out the current target completely and hides widgets
	if (CurrentMarkedTarget.IsValid())
	{
		CurrentMarkedTarget->SetVisibility(false, true);
		CurrentMarkedTarget = nullptr;
	}
	
	//CurrentMarkedTarget->Execute_OnUnMarked(this);
	
	
}

UMarkableComponent* UPlayerSubsystem::GetMarkedTarget() const
{
	//return the current marked target
	return CurrentMarkedTarget.Get();
}



