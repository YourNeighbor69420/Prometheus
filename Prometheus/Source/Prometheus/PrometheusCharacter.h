// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MarkableComponent.h"
#include "PlayerDamageInterface.h"
#include "PlayerSubsystem.h"
#include "Camera/CameraShakeBase.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "PrometheusCharacter.generated.h"

// Forward Declarations
class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
class UNiagaraSystem;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

//Delegates for UI and Animations
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSpeedUpdated, float, SpeedPercentage, bool, bIsMaxSpeed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInAttackRange, float, SkillCheckPercentage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAimCooldownUpdate, float, AimCooldownPercentage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeath);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSkillCheckSucceed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnemyMark);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnemyExecute);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSpeedDrain);


UCLASS(abstract)
class APrometheusCharacter : public ACharacter, public IPlayerDamageInterface
{
    GENERATED_BODY()

    //Skeletal mesh component for player animations 
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
    USkeletalMeshComponent* FirstPersonMesh;

    //First person camera component
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
    UCameraComponent* FirstPersonCameraComponent;

protected:
    //Input actions
    UPROPERTY(EditAnywhere, Category ="Input") UInputAction* JumpAction;
    UPROPERTY(EditAnywhere, Category ="Input") UInputAction* MoveAction;
    UPROPERTY(EditAnywhere, Category ="Input") UInputAction* LookAction;
    UPROPERTY(EditAnywhere, Category ="Input") UInputAction* MouseLookAction;
    UPROPERTY(EditAnywhere, Category ="Input") UInputAction* MarkAction;
    UPROPERTY(EditAnywhere, Category ="Input") UInputAction* AimAction;
    UPROPERTY(EditAnywhere, Category ="Input") UInputAction* AimReleaseAction;
    UPROPERTY(EditAnywhere, Category ="Input") UInputAction* AttackAction;
    UPROPERTY(EditAnywhere, Category ="Input") UInputAction* RestartAction;
    UPROPERTY(EditAnywhere, Category ="Input") UInputAction* DashAction;

    //Reference to audio component
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Audio")
    class UAudioComponent* MusicAudioComponent;

    //Function for player overlap with other actors 
    UFUNCTION()
    void OnPlayerContact(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

public:
    APrometheusCharacter();

    // Damage interface implementation
    virtual void ApplyPlayerDamage_Implementation(float SpeedDebuff) override;
    
    //Reference for camera effects and post processing
    UPROPERTY(EditDefaultsOnly, Category = "Camera") UMaterialInterface* SpeedBlurMaterial;
    UPROPERTY() UMaterialInstanceDynamic* SpeedBlurMaterialInstance;
    
    UPROPERTY(EditAnywhere, Category = "Camera") float MaxBlurIntensity = 0.05f;

    //Reference to the camera shake
    UPROPERTY(EditDefaultsOnly, Category="UI Events") TSubclassOf<UCameraShakeBase> DamageCameraShake;

    //Reference for attack niagara effect
    UPROPERTY(EditDefaultsOnly, Category="UI Events") UNiagaraSystem* AttackEffect;
    
protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    
    void MoveInput(const FInputActionValue& Value);
    void LookInput(const FInputActionValue& Value);

    //Input functions
    UFUNCTION(BlueprintCallable, Category="Input")
    virtual void DoAim(float Yaw, float Pitch);
    UFUNCTION(BlueprintCallable, Category="Input")
    virtual void DoMove(float Right, float Forward);
    UFUNCTION(BlueprintCallable, Category="Input")
    virtual void DoJumpStart();
    UFUNCTION(BlueprintCallable, Category="Input")
    virtual void DoJumpEnd();
    UFUNCTION(BlueprintCallable, Category="Input")
    void MarkInput();
    UFUNCTION(BlueprintCallable, Category="Input")
    void AimInput();
    UFUNCTION(BlueprintCallable, Category="Input")
    void AimReleaseInput();
    UFUNCTION(BlueprintCallable, Category="Input")
    void AttackInput();
    UFUNCTION(BlueprintCallable, Category="Input")
    void Attack(UMarkableComponent* Target);
    UFUNCTION(BlueprintCallable, Category="Input")
    void RestartInput();
    UFUNCTION(BlueprintCallable, Category="Input")
    void DashInput();
    
    //Reference for player subsystem
    UPROPERTY() UPlayerSubsystem* PlayerSubsystem;

    //Line trace for marking components
    FHitResult LineTrace();

    //Attack dash (formerly a teleport)
    void AttackTeleport(UMarkableComponent* Target);

    //Setting speed back to normal after an attack
    UFUNCTION() void EndAttack(UMarkableComponent* Target); 

public:
    //Event dispatchers
    UPROPERTY(BlueprintAssignable, Category = "UI Events")
    FOnAimCooldownUpdate OnAimCooldownUpdate;
    UPROPERTY(BlueprintAssignable, Category="UI Events")
    FOnSpeedUpdated OnSpeedUpdated;
    UPROPERTY(BlueprintAssignable, Category="UI Events")
    FOnDeath OnDeath;
    UPROPERTY(BlueprintAssignable, Category="UI Events")
    FOnInAttackRange OnInAttackRange;
    UPROPERTY(BlueprintAssignable, Category="UI Events")
    FOnSkillCheckSucceed OnSkillCheckSucceed;
    UPROPERTY(BlueprintAssignable, Category="UI Events")
    FOnSpeedDrain OnSpeedDrain;
    UPROPERTY(BlueprintAssignable, Category="Animations")
    FOnEnemyMark OnEnemyMark;
    UPROPERTY(BlueprintAssignable, Category="Animations")
    FOnEnemyExecute OnEnemyExecute;

    //Respawn player at checkpoint
    UFUNCTION(BlueprintCallable, Category="Checkpoints")
    void RespawnAtCheckpoint();

    //Reference for the current arena the player is in
    UPROPERTY() class AArenaManager* CurrentArenaManager;

    //Return how much damage the player has/does currently
    float GetDamage();
    
protected:
    virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

    //The players initial speed on spawn
    UPROPERTY(EditAnywhere, Category="Movement")
    float InitialLaunchSpeed = 5000.f;
    //Custom Speed
    UPROPERTY(EditAnywhere, Category="Movement")
    FVector ViLocity;
    //Max custom speed
    UPROPERTY(EditAnywhere, Category="Movement UI")
    float MaxViLocity = 5000.f;
    //How fast the player attack dash is
    UPROPERTY(EditAnywhere, Category="Movement")
    float DashSpeed = 20000.f;
    //How long the speed attack override lasts
    UPROPERTY(EditAnywhere, Category="Movement")
    float DashTimeLength = 0.2f;
    //The slowest the player can go before dying
    UPROPERTY(EditAnywhere, Category="Movement")
    float DeathSpeed = 0.100;
    //How fast the player loses speed
    UPROPERTY(EditAnywhere, Category="Movement")
    float Drag = 0.25f;

    //Normal change direction function
    void DashToTarget(UMarkableComponent* Target);
    
    //float MoveInputRight = 0.0f;
    
    //Storage for original speed values when attacking
    FVector OriginalViLocity; 
    float OriginalMaxViLocity;
    float OriginalDamage;

    //How much damage the player does
    float Damage;

    //Timer handles
    FTimerHandle DashTimerHandle;
    FTimerDelegate DashTimerDelegate;
    FTimerHandle AimCooldownTimerHandle;

    //How much slow motion to apply
    UPROPERTY(EditAnywhere, Category="Slow Motion")
    float TimeDilationFactor = 0.1f;
    //Volatile FOV variable
    float DesiredFOV;
    //Orignal FOV
    UPROPERTY(EditAnywhere, Category="Camera")
    float DefaultFOV = 100.f;
    //Slow motion FOV
    UPROPERTY(EditAnywhere, Category="Camera")
    float SlowMotionFOV = 60.f;
    
    //Volatile Sensitivity Variable
    float CurrentSensitivity = 1.f;
    //Original Sense
    UPROPERTY(EditAnywhere, Category="Camera")
    float DefaultSensitivity = 1.f;
    //Slow Motion Sense
    UPROPERTY(EditAnywhere, Category="Camera")
    float AimSensitivity = 0.4f;

    //Where the skill check begins
    UPROPERTY(EditAnywhere, Category="Combat")
    float DistanceToSkillCheck = 500.f;
    //Where the skill check ends
    UPROPERTY(EditAnywhere, Category="Combat")
    float MinimumDistance = 50.f;
    //Correct skill check zone minimum
    UPROPERTY(EditAnywhere, Category="Combat")
    float SkillCheckMinimum = 0.7;
    //Correct skill check zone maximum
    UPROPERTY(EditAnywhere, Category="Combat")
    float SkillCheckMaximum = 0.85;

    //Set cooldown false
    void TurnOffAimCooldown();

public:
    //Max FOV can be at max speed
    UPROPERTY(EditAnywhere, Category="Camera")
    float MaxSpeedFOV = 120.f;
    //How fast the FOV zooms
    UPROPERTY(EditAnywhere, Category="Camera")
    float FOVZoomSpeed = 10.f;

    //Returns skill check progress
    UFUNCTION(BlueprintPure, Category="Combat")
    float GetSkillCheckProgress();
    
    USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; }
    UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

    //Is the player aiming
    bool bIsAiming = false;
    //Aiming time
    float CurrentAimTime = 0.0f;

    //How long can you aim till it starts draining speed
    UPROPERTY(EditAnywhere, Category= "Aiming")
    float MaxSafeAimTime = 2.0f;
    //How fast does aiming drain speed
    UPROPERTY(EditAnywhere, Category="Aiming")
    float SpeedDrainRate = 1.f;

    //Is the cooldown on or off
    bool bAimCooldown = false;

    //How long till the player can aim again
    UPROPERTY(EditAnywhere, Category="Combat")
    float AimCooldownLength = 1.f;
    
    float CurrentCooldownTime = 0.0f;
};