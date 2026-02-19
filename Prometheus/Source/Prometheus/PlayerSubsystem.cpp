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

	if (CurrentMarkedTarget.IsValid())
	{
		IMarkingInterface::Execute_OnUnMarked(CurrentMarkedTarget.Get());
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow,"this does work");
	}

	CurrentMarkedTarget = NewMarkedTarget;
	
	if (NewMarkedTarget)
	{
		IMarkingInterface::Execute_OnMarked(NewMarkedTarget);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, FString::Printf(TEXT("Target Successfully marked: %s"), *NewMarkedTarget->GetName()));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, "BADDDDD");
	}

}

UMarkableComponent* UPlayerSubsystem::GetMarkedTarget() const
{
	return CurrentMarkedTarget.Get();
}



