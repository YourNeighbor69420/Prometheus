// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MarkableComponent.h"
#include "PlayerDamageInterface.h"
#include "PlayerSubsystem.h"
#include "Components/SphereComponent.h"
#include "Camera/CameraShakeBase.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"


#include "PrometheusCharacter.generated.h"


class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSpeedUpdated, float, SpeedPercentage, bool, bIsMaxSpeed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeath);

/**
 *  A basic first person character
 */
UCLASS(abstract)
class APrometheusCharacter : public ACharacter, public IPlayerDamageInterface
{
	GENERATED_BODY()

	/** Pawn mesh: first person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* FirstPersonMesh;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

protected:

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	class UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	class UInputAction* MouseLookAction;
	
	UPROPERTY(EditAnywhere, Category="Input")
	class UInputAction* MarkAction;

	UPROPERTY(EditAnywhere, Category="Input")
	class UInputAction* AimAction;

	UPROPERTY(EditAnywhere, Category="Input")
	class UInputAction* AimReleaseAction;

	UPROPERTY(EditAnywhere, Category="Input")
	class UInputAction* AttackAction;

	UPROPERTY(EditAnywhere, Category="Input")
	class UInputAction* RestartAction;

	UPROPERTY(EditAnywhere, Category="Input")
	class UInputAction* DashAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Audio")
	class UAudioComponent* MusicAudioComponent;

	UFUNCTION()
	void OnPlayerContact(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
public:
	APrometheusCharacter();

	virtual void ApplyPlayerDamage_Implementation(float SpeedDebuff) override;

protected:
	void BeginPlay() override;

	void Tick(float DeltaTime) override;
	
	/** Called from Input Actions for movement input */
	void MoveInput(const FInputActionValue& Value);

	/** Called from Input Actions for looking input */
	void LookInput(const FInputActionValue& Value);

	/** Handles aim inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoAim(float Yaw, float Pitch);

	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles jump start inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	/** Handles jump end inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();

	// Handles the player marking actors
	UFUNCTION(BlueprintCallable, Category="Input")
	void MarkInput();

	//Handles the slow motion aiming
	UFUNCTION(BlueprintCallable, Category="Input")
	void AimInput();

	//Knows when the aim is released
	UFUNCTION(BlueprintCallable, Category="Input")
	void AimReleaseInput();

	//Handles the players attack
	UFUNCTION(BlueprintCallable, Category="Input")
	void AttackInput();

	//Handles the players attack
	UFUNCTION(BlueprintCallable, Category="Input")
	void Attack(UMarkableComponent* Target);

	//Handles the level restart
	UFUNCTION(BlueprintCallable, Category="Input")
	void RestartInput();

	//Handles the players dashes
	UFUNCTION(BlueprintCallable, Category="Input")
	void DashInput();
	
	UPROPERTY()
	UPlayerSubsystem* PlayerSubsystem;

	//Returns whatever the player selects/clicks
	FHitResult LineTrace();

	//The teleport when attacking
	void AttackTeleport(UMarkableComponent* Target);

	

	//What is currently marked
	//TWeakObjectPtr<UMarkableComponent> CurrentMarkedTarget;

public:
	
	UPROPERTY(BlueprintAssignable, Category="UI Events")
	FOnSpeedUpdated OnSpeedUpdated;

	UPROPERTY(BlueprintAssignable, Category="UI Events")
	FOnDeath OnDeath;
	
	UPROPERTY(EditDefaultsOnly, Category="UI Events")
	TSubclassOf<UCameraShakeBase> DamageCameraShake; ;
	
	float GetDamage();
	
protected:

	/** Set up input action bindings */
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

	/*//Max Player speed
	UPROPERTY(EditAnywhere, Category="Movement")
	float MaxSpeed = 20000.f;*/

	//How fast the player is sent out at the beginning of the level
	UPROPERTY(EditAnywhere, Category="Movement")
	float InitialLaunchSpeed = 5000.f;

	void DashToTarget(UMarkableComponent* Target);

	float MoveInputRight = 0.0f;

	//Custom speed vector
	UPROPERTY(EditAnywhere, Category="Movement")
	FVector ViLocity;
	
	UPROPERTY(EditAnywhere, Category="Movement UI")
	float MaxUIViLocity = 5000.f;

	UPROPERTY(EditAnywhere, Category="Movement")
	float DeathSpeed = 0.100;
	
	float Damage;
	
	//Custom air resistance
	UPROPERTY(EditAnywhere, Category="Movement")
	float Drag = 0.25f;

	//How slow time goes when aiming
	UPROPERTY(EditAnywhere, Category="Slow Motion")
	float TimeDilationFactor = 0.1f;

	//What we currently want the fov to be
	float DesiredFOV;

	//The normal FOV
	UPROPERTY(EditAnywhere, Category="Camera")
	float DefaultFOV = 100.f;

	//The aiming FOV
	UPROPERTY(EditAnywhere, Category="Camera")
	float SlowMotionFOV = 60.f;

	//The current player sensitivity
	float CurrentSensitivity = 1.f;

	//The normal player sensitivity
	UPROPERTY(EditAnywhere, Category="Camera")
	float DefaultSensitivity = 1.f;

	//The sensitivity when aiming in
	UPROPERTY(EditAnywhere, Category="Camera")
	float AimSensitivity = 0.4f;

	//How close the player needs to be to attack an enemy
	UPROPERTY(EditAnywhere, Category="Combat")
	float DistanceToAttack = 300.f;

	//How far the teleport goes
	UPROPERTY(EditAnywhere, Category="Combat")
	float TeleportDistance = 200.f;

public:

	/** Returns the first person mesh **/
	USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; }

	/** Returns first person camera component **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }


};



