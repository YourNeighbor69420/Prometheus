// Copyright Epic Games, Inc. All Rights Reserved.

#include "PrometheusCharacter.h"

#include "ArenaManager.h"
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
#include "NiagaraFunctionLibrary.h"
#include "Prometheus.h"
#include "PrometheusGameInstance.h"
#include "Kismet/GameplayStatics.h"


void APrometheusCharacter::OnPlayerContact(UPrimitiveComponent* HitComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	OnDeath.Broadcast();
}

APrometheusCharacter::APrometheusCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
	
	// Create the Camera Component	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Camera"));
	FirstPersonCameraComponent->SetupAttachment(GetRootComponent());
	FirstPersonCameraComponent->SetRelativeLocationAndRotation(FVector(-2.8f, 5.89f, 0.0f), FRotator(0.0f, 90.0f, -90.0f));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	FirstPersonCameraComponent->bEnableFirstPersonFieldOfView = true;
	FirstPersonCameraComponent->bEnableFirstPersonScale = true;
	FirstPersonCameraComponent->FirstPersonFieldOfView = 70.0f;
	FirstPersonCameraComponent->FirstPersonScale = 0.6f;
	
	// Create the first person mesh that will be viewed only by this character's owner
	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("First Person Mesh"));
	FirstPersonMesh->SetupAttachment(FirstPersonCameraComponent);
	FirstPersonMesh->SetOnlyOwnerSee(true);
	FirstPersonMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;
	FirstPersonMesh->SetCollisionProfileName(FName("NoCollision"));
	
	//Music Component 
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

	//Subtract from player speed
	ViLocity *= SpeedDebuff;
	//Play camera shake to show damage
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
	
	//Check and create the speed blur material
	if (SpeedBlurMaterial && FirstPersonCameraComponent)
	{
		SpeedBlurMaterialInstance = UMaterialInstanceDynamic::Create(SpeedBlurMaterial, this);

		FirstPersonCameraComponent->AddOrUpdateBlendable(SpeedBlurMaterialInstance, 1.0f);
	}

	//Assign function if player makes contact
	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &APrometheusCharacter::OnPlayerContact);

	//Get player subsystem
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (ULocalPlayer* LocalPlayer = PC->GetLocalPlayer())
		{
			PlayerSubsystem = LocalPlayer->GetSubsystem<UPlayerSubsystem>();
		}
	}
	
	/*if (PlayerSubsystem)
	{
		UE_LOG(LogTemp, Display, TEXT("Player Subsystem Created"));
	}*/

	/*if (UCharacterMovementComponent* MoveComp =  GetCharacterMovement())
	{
		MoveComp->MaxFlySpeed = MaxSpeed;
	}
	LaunchCharacter(GetActorForwardVector() * InitialLaunchSpeed, true, true );*/

	//Deactivate normal movement 
	GetCharacterMovement()->Deactivate();

	//Launch player with custom movement and speed
	ViLocity = GetActorForwardVector() * InitialLaunchSpeed;

	//Set FOV
	GetFirstPersonCameraComponent()->FieldOfView = DefaultFOV;
	DesiredFOV = DefaultFOV;
	//Set Sensitivity
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

	///////////////////////////////////Aiming//////////////////////////////////////////
	//Check if player is aiming
	if (bIsAiming)
	{
		// If the player aims for too long, drain speed
		CurrentAimTime += DeltaTime;
		if (CurrentAimTime > MaxSafeAimTime)
		{
			ViLocity = ViLocity * (1.f - (SpeedDrainRate * DeltaTime));
			//Notify UI
			OnSpeedDrain.Broadcast();
		}
	}
	// Handle Cooldown recovery when not aiming
	else if (bIsAiming == false && bAimCooldown == true)
	{
		CurrentCooldownTime += DeltaTime;

		float CooldownPercentage = CurrentCooldownTime / AimCooldownLength;

		//Notify UI
		OnAimCooldownUpdate.Broadcast(CooldownPercentage);

		if (CurrentCooldownTime >= AimCooldownLength)
		{
			bAimCooldown = false;
			CurrentCooldownTime = 0.0f;
		}
	}

	//////////////////////////////////////////////Camera Effects/////////////////////////////////
	if (FirstPersonCameraComponent)
	{
		FVector2d SpeedRange(0.0f, MaxViLocity);
		FVector2D FOVRange(DefaultFOV, MaxSpeedFOV);

		// Map velocity to a target FOV
		float TargetFOV = FMath::GetMappedRangeValueClamped(SpeedRange, FOVRange, ViLocity.Length());
		// Smoothly interpolate the camera to the target FOV
		float SmoothedFOV = FMath::FInterpTo(FirstPersonCameraComponent->FieldOfView, TargetFOV, DeltaTime, FOVZoomSpeed);
		FirstPersonCameraComponent->SetFieldOfView(SmoothedFOV);

		// Apply Motion Blur intensity based on how fast we are going
		if (SpeedBlurMaterialInstance)
		{
			FVector2d BlurRange(0.0f, MaxBlurIntensity);
			float TargetBlur = FMath::GetMappedRangeValueClamped(SpeedRange, BlurRange, ViLocity.Length());
			SpeedBlurMaterialInstance->SetScalarParameterValue(FName("BlurIntensity"), TargetBlur);
		}
	}

	///////////////////////////////////////////Skill Checks////////////////////////////////
	if (UMarkableComponent* CurrentTarget = PlayerSubsystem->GetMarkedTarget())
	{
		float CurrentDistance = FVector::Distance(GetActorLocation(), CurrentTarget->GetComponentLocation());
        	
        	if (CurrentDistance <= DistanceToSkillCheck)
        	{
        		//Notify UI
        		OnInAttackRange.Broadcast(GetSkillCheckProgress());
        	}
	}
	
	//Steering system (broken)
	/*if (MoveInputRight != 0.0f)
	{

		float CurrentSpeed = ViLocity.Length();
		
		FVector RightVector = GetActorRightVector();
		float SteeringPower = 150.f;

		FVector SteeringNudge = (RightVector * MoveInputRight * SteeringPower * DeltaTime);
		ViLocity += SteeringNudge;

		ViLocity = ViLocity.GetSafeNormal() * CurrentSpeed;
	}*/

	/////////////////////////////////////Audio//////////////////////////////////
	//Update the music system based on speed
	if (MusicAudioComponent && MusicAudioComponent->IsPlaying())
	{
		MusicAudioComponent->SetFloatParameter(FName("PlayerSpeed"), ViLocity.Size());
	}

	///////////////////////////Speed///////////////////////////////////////////////////
	//Kill player if too slow
	if (ViLocity.Length() <= DeathSpeed)
	{
		//Notify UI
		OnDeath.Broadcast();
	}
	
	if (!ViLocity.IsNearlyZero())
	{
		// Apply passive air resistance
		ViLocity = ViLocity * (1.f - (Drag * DeltaTime));
		ViLocity = ViLocity.GetClampedToMaxSize(MaxViLocity);

		// Calculate damage dealt based on current speed
		Damage = ViLocity.Size() / 10.f;
		
		/*FString DebugMsg = FString::Printf(TEXT("Velocity: %s | Speed: %f, damage : %f"), 
				*ViLocity.ToString(), 
				ViLocity.Size(),
				Damage);*/
		
		//GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, DebugMsg);
		/*if (ViLocity.SizeSquared() < 1000.f)
		{
			ViLocity = FVector::ZeroVector;
		}*/

		// Broadcast percentage to UI
		if (MaxViLocity > 0.0f)
		{
			float CurrentSpeed = ViLocity.Size();

			float Percentage = FMath::Clamp(CurrentSpeed / MaxViLocity, 0.0f, 1.0f);

			bool bIsMaxSpeed = (Percentage >= 0.99f);

			OnSpeedUpdated.Broadcast(Percentage, bIsMaxSpeed);
		}
	}

	// Physically move the character based on current ViLocity
	if (!ViLocity.IsNearlyZero())
	{
		FVector DesiredMove = ViLocity * DeltaTime;
		AddActorWorldOffset(DesiredMove, true);
		
	}


	////////////////////////////Slow Motion///////////////////////////////////////
	//Change FOV in relation to time dilation
	float CurrentFOV = GetFirstPersonCameraComponent()->FieldOfView;
	
	if (FMath::IsNearlyEqual(CurrentFOV, DesiredFOV, 0.1f))
	{
		return;
	}

	float TimeDilation = UGameplayStatics::GetGlobalTimeDilation(this);
	float RealDeltaTime;

	// Adjust DeltaTime manually if time is slowed to keep FOV transition smooth
	if (TimeDilation < 0.9f)
	{
		RealDeltaTime = 0.02f;
	}
	else
	{
		RealDeltaTime = DeltaTime;
	}

	//Actually set the FOV
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

		//Marking
		EnhancedInputComponent->BindAction(MarkAction, ETriggerEvent::Started, this, &APrometheusCharacter::MarkInput);

		//Aiming
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &APrometheusCharacter::AimInput);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &APrometheusCharacter::AimReleaseInput);

		//Attack
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &APrometheusCharacter::AttackInput);

		//Restart
		EnhancedInputComponent->BindAction(RestartAction, ETriggerEvent::Started, this, &APrometheusCharacter::RestartInput);

		//Dash
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
	FHitResult Hit = LineTrace();
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Mark input" );

	
	//If hitting a markable component, set marked target and show widget
	if (Hit.bBlockingHit)
	{
		if (UMarkableComponent* HitComponent = Cast<UMarkableComponent>(Hit.GetComponent()))
		{
			if (PlayerSubsystem)
			{
				PlayerSubsystem->SetMarkedTarget(HitComponent);
				OnEnemyMark.Broadcast();
			}
		}
	}
}

void APrometheusCharacter::AimInput()
{

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Aim input" );
	
	//Slow down the worlds time and change the FOV

	//Check cooldown
	if (bAimCooldown == false)
	{
		//Aim in, slow down time, change FOV
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), TimeDilationFactor);
		DesiredFOV = SlowMotionFOV;
		CurrentSensitivity = AimSensitivity;
		
		bIsAiming = true;
	}
	
	//GetFirstPersonCameraComponent()->FieldOfView = SlowMotionFOV;
}

void APrometheusCharacter::AimReleaseInput()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Aim input" );
	
	//Sets the worlds time and FOV back to normal
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.f);
	DesiredFOV = DefaultFOV;
	CurrentSensitivity = DefaultSensitivity;

	//Don't let the player call the release aim function if the cooldown is active
	if (bAimCooldown == false)
	{
		bIsAiming = false;
     	CurrentAimTime = 0.f;
		//GetWorld()->GetTimerManager().SetTimer(AimCooldownTimerHandle, this, &APrometheusCharacter::TurnOffAimCooldown, AimCooldownLength, false);
		bAimCooldown = true;
	}
	
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
		OnEnemyExecute.Broadcast();
	}
	//If the damage isn't above half the enemy's current health, take a speed penalty
	else if (Damage < Target->GetDamageThreshold_Implementation())
	{
		ViLocity = ViLocity * Target->GetSpeedPenalty_Implementation();
		Target->DealDamage_Implementation(Damage);
	}
	//If you didn't kill it instantly and did do more than half its health, deal damage and get a normal speed reward
	else
	{
		ViLocity = ViLocity * Target->GetSpeedReward_Implementation();
		Target->DealDamage_Implementation(Damage);
	}
	PlayerSubsystem->ClearMarkedTarget();

	
}

void APrometheusCharacter::RestartInput()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Restart input" );
}

void APrometheusCharacter::DashInput()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Dash input" );

	//If there is an alive, marked target, dash towards the component
	if (UMarkableComponent* MarkedTarget = PlayerSubsystem->GetMarkedTarget())
	{
		if (AEnemyPawn* TargetEnemy = Cast<AEnemyPawn>(MarkedTarget->GetOwner()))
        	{
        		if (MarkedTarget)
                	{
                		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Dash input" );
                		if (TargetEnemy->bIsAlive)
                		{
                			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Dash worked" );
                			DashToTarget(MarkedTarget);
                		}
                	}
        	}
	}
	else
	{
		MarkedTarget = nullptr;
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
		//DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 5.0f);

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
	
	
	/*FVector PlayerLocation = GetActorLocation();
	FVector TargetLocation = Target->GetComponentLocation();
	float Distance = FVector::Dist(PlayerLocation, TargetLocation);*/

	
	
	//Get the skill check progress between the player and the marked target and check if we are close enough to attack/teleport
	float CurrentProgress = GetSkillCheckProgress();
	if (CurrentProgress >= SkillCheckMinimum && CurrentProgress <= SkillCheckMaximum)
	{
		//Notify UI
		OnSkillCheckSucceed.Broadcast();
		//Phase through any enemies
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		//Store current movement stats to restore them after the dash
		OriginalViLocity = ViLocity;
		OriginalMaxViLocity = MaxViLocity;
		OriginalDamage = Damage;

		//Temporarily override max speed and set velocity to dash speed
		MaxViLocity = DashSpeed;
		ViLocity = ViLocity * DashSpeed;

		//Spawn the Niagara attack effect
		if ( AttackEffect)
		{
			UNiagaraFunctionLibrary::SpawnSystemAttached(AttackEffect, GetRootComponent(), NAME_None, FVector(0.f, 0.f, 0.f), FRotator(0.f, 90.f, 0.f), EAttachLocation::KeepRelativeOffset, true);
		}
		
		//Get the direction from the player to the enemy
		//FVector ApproachDirection = (TargetLocation - PlayerLocation).GetSafeNormal();
		
		//Start at the enemy and teleport the teleport distance
		//FVector TeleportDestination = TargetLocation + (ApproachDirection * TeleportDistance);
		
		AActor* EnemyActor = Target->GetOwner();

		//Ignore the enemy actor when teleporting as to not collide with them
		GetCapsuleComponent()->IgnoreActorWhenMoving(EnemyActor, true);
		
		//Teleport to the destination we have set while checking if we pass through anything
		//SetActorLocation(TeleportDestination, true);
		DashTimerDelegate.BindUObject(this, &APrometheusCharacter::EndAttack, Target);

		//Set a timer to end the dash and execute the damage logic
		GetWorld()->GetTimerManager().SetTimer(DashTimerHandle, DashTimerDelegate, DashTimeLength, false);
		
		//GEngine->AddOnScreenDebugMessage(-1, 5.f , FColor::Emerald, "teleported");
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Emerald, FString::Printf(TEXT("No collision on: %s"), *Target->GetOwner()->GetName()));
		//Re-enable collision on the player
		GetCapsuleComponent()->IgnoreActorWhenMoving(EnemyActor, false);
	}
}

void APrometheusCharacter::EndAttack(UMarkableComponent* Target)
{
	// Restore original movement state
	MaxViLocity = OriginalMaxViLocity;
	ViLocity = OriginalViLocity;
	Damage = OriginalDamage;

	// Apply actual damage to the target component
	Attack(Target);
	
	// Restore collision so we don't stay invincible
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
}

void APrometheusCharacter::RespawnAtCheckpoint()
{
	//Get game instance
	UPrometheusGameInstance* GameInstance = Cast<UPrometheusGameInstance>(GetGameInstance());
	 if (GameInstance && GameInstance->bHasSavedCheckpoint)
	 {
	 	//Move player to the saved transform
		 SetActorLocationAndRotation(GameInstance->SavedLocation, GameInstance->SavedRotation, false, nullptr, ETeleportType::TeleportPhysics);

	 	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	 	{
	 		//Set rotation and speed
	 		PC->SetControlRotation(GameInstance->SavedRotation);
	 		ViLocity = ViLocity * 0.f;;
	 		ViLocity = GameInstance->SavedRotation.Vector() * InitialLaunchSpeed;

	 		//Reset world
	 		CurrentArenaManager->ResetArena();
	 		PlayerSubsystem->ClearMarkedTarget();
	 		
	 	}
	 }
}

float APrometheusCharacter::GetDamage()
{
	return Damage;
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

void APrometheusCharacter::TurnOffAimCooldown()
{
	bAimCooldown = false;
}

float APrometheusCharacter::GetSkillCheckProgress()
{
	//Get distance between the player and component
	UMarkableComponent* CurrentTarget = PlayerSubsystem->GetMarkedTarget();
	float CurrentDistance = FVector::Distance(GetActorLocation(), CurrentTarget->GetComponentLocation());

	//Map the skill check minimum distance and its begining and turn that into a value between 0 and 1 
	FVector2D DistanceRange(DistanceToSkillCheck, MinimumDistance);
	FVector2D UIRange(0.0f, 1.0f);

	return FMath::GetMappedRangeValueClamped(DistanceRange, UIRange, CurrentDistance);
}
