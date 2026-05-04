// Fill out your copyright notice in the Description page of Project Settings.


#include "Doors.h"

#include "ArenaManager.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ADoors::ADoors()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	DoorsMesh=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door Mesh"));
	RootComponent = DoorsMesh;
	
	DoorsMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);


}

// Called when the game starts or when spawned
void ADoors::BeginPlay()
{
	Super::BeginPlay();

	//Check is an arena is set as the door manager
	if (DoorsManager)
	{
		//Listen for when the arena delegate is broadcasted to unlock the door
		DoorsManager->OnArenaFinished.AddDynamic(this, &ADoors::UnlockDoor);
	}
}

void ADoors::UnlockDoor()
{
	//Turn off collision and make it invisible
	DoorsMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DoorsMesh->SetVisibility(false);
}

// Called every frame
void ADoors::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

