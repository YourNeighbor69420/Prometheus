// Fill out your copyright notice in the Description page of Project Settings.


#include "Checkpoint.h"
#include "Components/BoxComponent.h"
#include "PrometheusCharacter.h"
#include "PrometheusGameInstance.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ACheckpoint::ACheckpoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	RootComponent = BoxComponent;
	BoxComponent->SetCollisionProfileName(TEXT("Trigger"));

	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ACheckpoint::OnOverlapBegin);

}

// Called when the game starts or when spawned
void ACheckpoint::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACheckpoint::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<APrometheusCharacter>(OtherActor))
	{
		UPrometheusGameInstance* GameInstance = Cast<UPrometheusGameInstance>(UGameplayStatics::GetGameInstance(this));

		if (GameInstance)
		{
			GameInstance->bHasSavedCheckpoint = true;
			GameInstance->SavedLocation = GetActorLocation();
			GameInstance->SavedRotation = GetActorRotation();

			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Silver, "Hit new check point");
		}
	}
	
}

// Called every frame
void ACheckpoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

