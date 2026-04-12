// Fill out your copyright notice in the Description page of Project Settings.


#include "Doors.h"

#include "ArenaManager.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ADoors::ADoors()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DoorsMesh=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door Mesh"));
	RootComponent = DoorsMesh;
	
	DoorsMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);


}

// Called when the game starts or when spawned
void ADoors::BeginPlay()
{
	Super::BeginPlay();


	if (DoorsManager)
	{
		DoorsManager->OnArenaFinished.AddDynamic(this, &ADoors::UnlockDoor);
	}
}

void ADoors::UnlockDoor()
{
	DoorsMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called every frame
void ADoors::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

