// Copyright Epic Games, Inc. All Rights Reserved.

#include "PrometheusCharacter.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PlayerSubsystem.h"
#include "MarkingInterface.h"
#include "Prometheus.h"

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

	// configure the character comps
	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::WorldSpaceRepresentation;

	GetCapsuleComponent()->SetCapsuleSize(34.0f, 96.0f);

	// Configure character movement
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	GetCharacterMovement()->AirControl = 0.5f;
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
	/*if (PlayerSubsystem)
	{
		UE_LOG(LogTemp, Display, TEXT("Player Subsystem Created"));
	}*/
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
		
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Ongoing, this, &APrometheusCharacter::AimInput);
		
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

	// pass the axis values to the move input
	DoMove(MovementVector.X, MovementVector.Y);

}

void APrometheusCharacter::LookInput(const FInputActionValue& Value)
{
	// get the Vector2D look axis
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// pass the axis values to the aim input
	DoAim(LookAxisVector.X, LookAxisVector.Y);

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
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Mark input" );
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
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Aim input" );
}

void APrometheusCharacter::AttackInput()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Attack input" );
}

void APrometheusCharacter::RestartInput()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Restart input" );
}

void APrometheusCharacter::DashInput()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Dash input" );
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
		FVector End = Start + (CameraRotation.Vector() * 10000.f);

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
