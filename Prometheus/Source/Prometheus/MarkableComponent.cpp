// Fill out your copyright notice in the Description page of Project Settings.


#include "MarkableComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "EnemyPoolSubsystem.h"
#include "MovieSceneTracksComponentTypes.h"
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

	MarkWidget->AttachToComponent(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	if (ExecutableEffectSystem)
	{
		ExecutableEffectComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(ExecutableEffectSystem, this, NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, false);
		 if (ExecutableEffectComponent)
		 {
			 ExecutableEffectComponent->Deactivate();
		 }
	}
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
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("damage dealt: %f"), damage));
	
	Health -= damage;
	Health = FMath::Clamp(Health, 0, MaxHealth);
	
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("enemy health: %f / %f"), Health, MaxHealth));

	
	if (Health == 0.f)
	{
		UEnemyPoolSubsystem* EnemyPoolSubsystem = GetWorld()->GetSubsystem<UEnemyPoolSubsystem>();
		if (EnemyPoolSubsystem)
		{

			AActor* OwnerActor = GetOwner();
			AEnemyPawn* EnemyPawn = Cast<AEnemyPawn>(OwnerActor);
			EnemyPoolSubsystem->ReturnEnemy(EnemyPawn);
		}
		
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

void UMarkableComponent::ResetHealth_Implementation()
{
	IMarkingInterface::ResetHealth_Implementation();

	Health = MaxHealth;
}

void UMarkableComponent::SetExecutableEffectActive(bool bIsEffectActive)
{
	if (ExecutableEffectComponent)
	{
		if (bIsEffectActive && !ExecutableEffectComponent->IsActive())
		{
			ExecutableEffectComponent->Activate();
		}
		else if (!bIsEffectActive && ExecutableEffectComponent->IsActive())
		{
			ExecutableEffectComponent->Deactivate();
		}
	}
}



