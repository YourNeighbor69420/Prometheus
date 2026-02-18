// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MarkableComponent.h"
#include "PlayerSubsystem.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"


#include "PrometheusCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  A basic first person character
 */
UCLASS(abstract)
class APrometheusCharacter : public ACharacter
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
	
	
public:
	APrometheusCharacter();

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

	//What is currently marked
	TWeakObjectPtr<UMarkableComponent> CurrentMarkedTarget;

protected:

	/** Set up input action bindings */
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

	//Max Player speed
	UPROPERTY(EditAnywhere, Category="Movement")
	float MaxSpeed = 20000.f;

	//How fast the player is sent out at the beginning of the level
	UPROPERTY(EditAnywhere, Category="Movement")
	float InitialLaunchSpeed = 5000.f;

	void DashToTarget(UMarkableComponent* Target);

	//Custom speed
	UPROPERTY(EditAnywhere, Category="Movement")
	FVector ViLocity;
	//Custom air resistance
	UPROPERTY(EditAnywhere, Category="Movement")
	float Drag = 0.5f;

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

public:

	/** Returns the first person mesh **/
	USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; }

	/** Returns first person camera component **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

};



