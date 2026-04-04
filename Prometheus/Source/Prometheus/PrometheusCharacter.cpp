// Copyright Epic Games, Inc. All Rights Reserved.

#include "PrometheusCharacter.h"

#include "EnemyPawn.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/AudioComponent.h"
#include "PlayerSubsystem.h"
#include "MarkingInterface.h"
#include "Prometheus.h"
#include "Kismet/GameplayStatics.h"
#include "Math/UnitConversion.h"

APrometheusCharacter::APrometheusCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
	
	// Create the first person mesh that will be viewed only by this character's owner
	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("First Person Mesh"));

	FirstPersonMesh->SetupAttachment(GetMesh());
	FirstPersonMesh->SetOnlyOwnerSee(true);
	FirstPersonMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;
	FirstPersonMesh->SetCollisionProfileName(FName("NoCollision"));

	// Create the Camera Component	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Camera"));
	FirstPersonCameraComponent->SetupAttachment(FirstPersonMesh, FName("head"));
	FirstPersonCameraComponent->SetRelativeLocationAndRotation(FVector(-2.8f, 5.89f, 0.0f), FRotator(0.0f, 90.0f, -90.0f));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	FirstPersonCameraComponent->bEnableFirstPersonFieldOfView = true;
	FirstPersonCameraComponent->bEnableFirstPersonScale = true;
	FirstPersonCameraComponent->FirstPersonFieldOfView = 70.0f;
	FirstPersonCameraComponent->FirstPersonScale = 0.6f;

	MusicAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("MusicAudio"));
	MusicAudioComponent->SetupAttachment(RootComponent);

	// configure the character comps
	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::WorldSpaceRepresentation;

	GetCapsuleComponent()->SetCapsuleSize(34.0f, 96.0f);

	// Configure character movement
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	GetCharacterMovement()->AirControl = 0.5f;
}

void APrometheusCharacter::ApplyPlayerDamage_Implementation(float SpeedDebuff)
{
	IPlayerDamageInterface::ApplyPlayerDamage_Implementation(SpeedDebuff);

	ViLocity *= SpeedDebuff;
	if (DamageCameraShake)
	{
		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			PC->ClientStartCameraShake(DamageCameraShake);
		}
			
	}
}

void APrometheusCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (ULocalPlayer* LocalPlayer = PC->GetLocalPlayer())
		{
			PlayerSubsystem = LocalPlayer->GetSubsystem<UPlayerSubsystem>();
		}
	}
	float number = 1.f;
	/*if (PlayerSubsystem)
	{
		UE_LOG(LogTemp, Display, TEXT("Player Subsystem Created"));
	}*/

	/*if (UCharacterMovementComponent* MoveComp =  GetCharacterMovement())
	{
		MoveComp->MaxFlySpeed = MaxSpeed;
	}
	LaunchCharacter(GetActorForwardVector() * InitialLaunchSpeed, true, true );*/

	GetCharacterMovement()->Deactivate();
	ViLocity = GetActorForwardVector() * InitialLaunchSpeed;

	//Set FOV
	GetFirstPersonCameraComponent()->FieldOfView = DefaultFOV;
	DesiredFOV = DefaultFOV;

	CurrentSensitivity = DefaultSensitivity;
}

void APrometheusCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*const FVector Direction = GetActorForwardVector();

	if (UCharacterMovementComponent* MoveComp =  GetCharacterMovement())
	{
		ViLocity = MoveComp->Velocity;
	}*/
	
	//AddMovementInput(Direction, 1.0f);

	if (MoveInputRight != 0.0f)
	{

		float CurrentSpeed = ViLocity.Length();
		
		FVector RightVector = GetActorRightVector();
		float SteeringPower = 150.f;

		FVector SteeringNudge = (RightVector * MoveInputRight * SteeringPower * DeltaTime);
		ViLocity += SteeringNudge;

		ViLocity = ViLocity.GetSafeNormal() * CurrentSpeed;
	}
	
	if (MusicAudioComponent && MusicAudioComponent->IsPlaying())
	{
		MusicAudioComponent->SetFloatParameter(FName("PlayerSpeed"), ViLocity.Size());
	}
	
	if (!ViLocity.IsNearlyZero())
	{
		ViLocity = ViLocity * (1.f - (Drag * DeltaTime));
		ViLocity = ViLocity.GetClampedToMaxSize(5000.f);
		Damage = ViLocity.Size() / 10.f;
		
		FString DebugMsg = FString::Printf(TEXT("Velocity: %s | Speed: %f, damage : %f"), 
				*ViLocity.ToString(), 
				ViLocity.Size(),
				Damage);
		
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, DebugMsg);
		/*if (ViLocity.SizeSquared() < 1000.f)
		{
			ViLocity = FVector::ZeroVector;
		}*/

		if (MaxUIViLocity > 0.0f)
		{
			float CurrentSpeed = ViLocity.Size();

			float Percentage = FMath::Clamp(CurrentSpeed / MaxUIViLocity, 0.0f, 1.0f);

			bool bIsMaxSpeed = (Percentage >= 0.99f);

			OnSpeedUpdated.Broadcast(Percentage, bIsMaxSpeed);
		}
	}

	if (!ViLocity.IsNearlyZero())
	{
		FVector DesiredMove = ViLocity * DeltaTime;
		AddActorWorldOffset(DesiredMove, true);
		
	}
	
	float CurrentFOV = GetFirstPersonCameraComponent()->FieldOfView;
	
	if (FMath::IsNearlyEqual(CurrentFOV, DesiredFOV, 0.1f))
	{
		return;
	}

	float TimeDilation = UGameplayStatics::GetGlobalTimeDilation(this);
	float RealDeltaTime;

	if (TimeDilation < 0.9f)
	{
		RealDeltaTime = 0.02f;
	}
	else
	{
		RealDeltaTime = DeltaTime;
	}

	
	float NewFOV = FMath::FInterpTo(CurrentFOV, DesiredFOV, RealDeltaTime, 10.f);
	GetFirstPersonCameraComponent()->SetFieldOfView(NewFOV);
}

void APrometheusCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &APrometheusCharacter::DoJumpStart);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &APrometheusCharacter::DoJumpEnd);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APrometheusCharacter::MoveInput);

		// Looking/Aiming
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APrometheusCharacter::LookInput);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &APrometheusCharacter::LookInput);

		
		EnhancedInputComponent->BindAction(MarkAction, ETriggerEvent::Started, this, &APrometheusCharacter::MarkInput);
		
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &APrometheusCharacter::AimInput);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &APrometheusCharacter::AimReleaseInput);
		
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &APrometheusCharacter::AttackInput);
		
		EnhancedInputComponent->BindAction(RestartAction, ETriggerEvent::Started, this, &APrometheusCharacter::RestartInput);
		
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Started, this, &APrometheusCharacter::DashInput);
	}
	else
	{
		UE_LOG(LogPrometheus, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}


void APrometheusCharacter::MoveInput(const FInputActionValue& Value)
{
	// get the Vector2D move axis
	FVector2D MovementVector = Value.Get<FVector2D>();

	MoveInputRight = MovementVector.X;
	
	// pass the axis values to the move input
	//DoMove(MovementVector.X, MovementVector.Y);

}

void APrometheusCharacter::LookInput(const FInputActionValue& Value)
{
	// get the Vector2D look axis
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// pass the axis values to the aim input
	DoAim(LookAxisVector.X * CurrentSensitivity, LookAxisVector.Y * CurrentSensitivity);

}

void APrometheusCharacter::DoAim(float Yaw, float Pitch)
{
	if (GetController())
	{
		// pass the rotation inputs
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void APrometheusCharacter::DoMove(float Right, float Forward)
{
	if (GetController())
	{
		// pass the move inputs
		AddMovementInput(GetActorRightVector(), Right);
		AddMovementInput(GetActorForwardVector(), Forward);
	}
}

void APrometheusCharacter::DoJumpStart()
{
	// pass Jump to the character
	Jump();
}

void APrometheusCharacter::DoJumpEnd()
{
	// pass StopJumping to the character
	StopJumping();
}

void APrometheusCharacter::MarkInput()
{
	//Check if the line trace hit a markable component and sets it as the current marked target
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Mark input" );
	FHitResult Hit = LineTrace();

	if (Hit.bBlockingHit)
	{
		if (UMarkableComponent* HitComponent = Cast<UMarkableComponent>(Hit.GetComponent()))
		{
			if (PlayerSubsystem)
			{
				PlayerSubsystem->SetMarkedTarget(HitComponent);
			}
		}
	}
}

void APrometheusCharacter::AimInput()
{
	//Slow down the worlds time and change the FOV
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Aim input" );
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), TimeDilationFactor);
	DesiredFOV = SlowMotionFOV;
	CurrentSensitivity = AimSensitivity;
	//GetFirstPersonCameraComponent()->FieldOfView = SlowMotionFOV;
}

void APrometheusCharacter::AimReleaseInput()
{
	//Sets the worlds time and FOV back to normal
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Aim input" );
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.f);
	DesiredFOV = DefaultFOV;
	CurrentSensitivity = DefaultSensitivity;
	//GetFirstPersonCameraComponent()->FieldOfView = DefaultFOV;

	
}

void APrometheusCharacter::AttackInput()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Attack input" );
	if (PlayerSubsystem->GetMarkedTarget())
	{
		//Teleport behind the target
		AttackTeleport(PlayerSubsystem->GetMarkedTarget());
	}
}

void APrometheusCharacter::Attack(UMarkableComponent* Target)
{
	
	//AttackTeleport(Target);

	//If the damage can kill the enemy, execute it and give a large speed reward
	if (Damage >= Target->GetHealth_Implementation())
	{
		ViLocity = ViLocity * Target->GetExecuteSpeedReward_Implementation();
		Target->DealDamage_Implementation(Damage);
	}
	//If the damage isn't above half the enemy's current health, take a speed penalty
	else if (Damage < Target->GetDamageThreshold_Implementation())
	{
		ViLocity = ViLocity * Target->GetSpeedPenalty_Implementation();
	}
	//If you didn't kill it instantly and did do more than half its health, deal damage and get a normal speed reward
	else
	{
		ViLocity = ViLocity * Target->GetSpeedReward_Implementation();
		Target->DealDamage_Implementation(Damage);
	}
}

void APrometheusCharacter::RestartInput()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Restart input" );
}

void APrometheusCharacter::DashInput()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Dash input" );

	UMarkableComponent* MarkedTarget = PlayerSubsystem ? PlayerSubsystem->GetMarkedTarget(): nullptr;

	AEnemyPawn* TargetEnemy = Cast<AEnemyPawn>(MarkedTarget->GetOwner());
	
	if (MarkedTarget)
	{
		if (TargetEnemy->bIsAlive)
		{
			DashToTarget(MarkedTarget);
		}
	}
}

FHitResult APrometheusCharacter::LineTrace()
{
	FHitResult Hit;
	
	//Get Player Controller and Camera View Data
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		FVector CameraLocation;
		FRotator CameraRotation;

		//Get Camera View
		PC->GetPlayerViewPoint(CameraLocation, CameraRotation);

		//Get camera location and rotation and make line trace X units away
		FVector Start = CameraLocation;
		FVector End = Start + (CameraRotation.Vector() * 20000.f);

		//Set collision to not hit self
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);

		//Line trace and return data as Hit
		bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);
		//Show line trace visually
		DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 5.0f);

		//If hitting actor, check components to see if they implement marking, if so call function
		if (bHit)
		{
			UActorComponent* HitComponent = Hit.GetComponent();
			if (HitComponent && HitComponent->Implements<UMarkingInterface>())
			{
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "LineTrace");
				IMarkingInterface::Execute_OnMarked(HitComponent);
			}
		}
	}
	
	return Hit;
}

void APrometheusCharacter::AttackTeleport(UMarkableComponent* Target)
{
	//Get the distance between the player and the marked target and check if we are close enough to attack/teleport
	FVector PlayerLocation = GetActorLocation();
	FVector TargetLocation = Target->GetComponentLocation();
	float Distance = FVector::Dist(PlayerLocation, TargetLocation);

	if (Distance < DistanceToAttack)
	{
		//Get the direction from the player to the enemy
		FVector ApproachDirection = (TargetLocation - PlayerLocation).GetSafeNormal();
		//Start at the enemy and teleport the teleport distance
		FVector TeleportDestination = TargetLocation + (ApproachDirection * TeleportDistance);
		AActor* EnemyActor = Target->GetOwner();

		//Ignore the enemy actor when teleporting as to not collide with them
		GetCapsuleComponent()->IgnoreActorWhenMoving(EnemyActor, true);
		//Teleport to the destination we have set while checking if we pass through anything
		SetActorLocation(TeleportDestination, true);
		//GEngine->AddOnScreenDebugMessage(-1, 5.f , FColor::Emerald, "teleported");
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Emerald, FString::Printf(TEXT("No collision on: %s"), *Target->GetOwner()->GetName()));
		//Re-enable collision on the player
		GetCapsuleComponent()->IgnoreActorWhenMoving(EnemyActor, false);

		Attack(Target);

	}

	
}

void APrometheusCharacter::DashToTarget(UMarkableComponent* Target)
 {
	 if (!Target) return;

	//If theres no target to dash to, dont. Otherwise find the distance between actor and component and change direction to face it
	FVector Start = GetActorLocation();
	FVector End = Target->GetComponentLocation();
	FVector Direction = (End - Start).GetSafeNormal();
	//Face the actor in the correct direction
	SetActorRotation(Direction.Rotation());
	//keep the same speed but change where we are directed
	ViLocity = Direction * ViLocity.Size();

	//Direction = FVector::ZeroVector;
	/*
	UCharacterMovementComponent* MoveComp = GetCharacterMovement();

	MoveComp->Velocity = Direction * 1000.f;*/
	
 }
