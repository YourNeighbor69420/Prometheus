// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/SphereComponent.h"
#include "PlayerDamageInterface.h"
#include "Projectile.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::AProjectile::OnProjectileOverlap);

}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

void AProjectile::OnProjectileOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if  (OtherActor->Implements<UPlayerDamageInterface>())
	{
		IPlayerDamageInterface::Execute_ApplyPlayerDamage(OtherActor, ProjectileDamage);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, TEXT("Projectile hit the Player"));
	}
}
// Called every frame
void AProjectile::Tick(float DeltaTime)
{

	Super::Tick(DeltaTime);

}

