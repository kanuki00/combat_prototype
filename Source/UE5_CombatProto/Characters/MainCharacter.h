// Copyright Kalevi Toivanen 2022.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "Animation/AnimMontage.h"
#include "BaseCharacter.h"

#include "MainCharacter.generated.h"

// Interface for animations
UINTERFACE(MinimalAPI, Blueprintable)
class UCharacterAnimationInterface : public UInterface
{
	GENERATED_BODY()
};

class UE5_COMBATPROTO_API ICharacterAnimationInterface
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Animation Interface")
		float GetMovementInputStrength();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Animation Interface")
		FVector GetMovementInputDirection();
};

// Main character class
UCLASS()
class UE5_COMBATPROTO_API AMainCharacter : public ABaseCharacter, public ICharacterAnimationInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMainCharacter();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool MainCharacterDebug = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int MainCharacterDebugLevel = -1;

	////////////////////////////////////////////
	//****** Interface implementations *******//
	////////////////////////////////////////////

	// Implementation for character animation interface
	float GetMovementInputStrength_Implementation();
	FVector GetMovementInputDirection_Implementation();

	// Implementation for combat interface.
	virtual void WeaponHit(AActor* HitActor) override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Component pointers
	UPROPERTY(EditAnywhere)
		UCameraComponent* Camera;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USpringArmComponent* CameraBoom;

	// Attack Montages
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animations")
		UAnimMontage* FastAttack = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animations")
		UAnimMontage* StrongAttack = nullptr;

	// Input axis/action names
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Names")
		FName MoveForwardName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Names")
		FName MoveRightName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Names")
		FName LookUpName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Names")
		FName LookRightName;

	// Movement input data
	UPROPERTY(BlueprintReadOnly, Category = "Data")
		float MovementInputStrength = 0.0f;
	UPROPERTY(BlueprintReadOnly, Category = "Data")
		FVector MovementInputVector;

	// Camera input data
	UPROPERTY(BlueprintReadOnly, Category = "Data")
		FVector CameraInputVector;

	// Movement switch
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Switches")
		bool PlayerHasMovementControl = true;
	// Camera switch
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Switches")
		bool PlayerHasCameraControl = true;
	// Character facing switch
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Switches")
		bool PlayerHasRotationControl = true;

	virtual void SetCharacterRotationEnabled(bool NewRotate = true) override;

	////////////////////////////////////////////
	//** Attack logic methods and variables **//
	////////////////////////////////////////////

	

	bool CanStartFastAttack = true;
	bool CanStartStrongAttack = true;

	bool FastAttackCoolingDown = false;
	bool StrongAttackCoolingDown = false;
	float FastAttackCooldownTimer = 0.0f;
	float StrongAttackCooldownTimer = 0.0f;

	bool ShouldContinueFastAttack = false;
	bool ShouldContinueStrongAttack = false;

	FName CurrentFastAttackSection;
	FName CurrentFastAttackSectionCache;
	FName CurrentStrongAttackSection;
	FName CurrentStrongAttackSectionCache;

	float FastAttackCoolDownLength = 0.18f;
	float StrongAttackCoolDownLength = 0.3f;

	void StartFastAttack();
	void StartStrongAttack();

	void CheckAttackPressed();
	
	void ContinueAttack();

	void TransToStrongAttack();
	void TransToFastAttack();

	void EndAttack();

	///////////////////////////
	// ***** Targeting ***** //
	///////////////////////////

	// actor class that the character can target
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		TSubclassOf<AActor> TargetedActorClass;
	// list of all possible targets in the level
	TArray<AActor*> AllTargets;
	// list of all visible targets in the level
	TArray<AActor*> AllVisibleTargets;

	// Pointer to target
	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
		AActor* Target;

	// Pointer to target suggested by targeting logic. Used to draw suggestion widget.
	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
		AActor* SuggestedTarget;

	UPROPERTY(BlueprintReadOnly) // Visible to blueprint so that anim bp knows when to strafe.
		bool IsTargeting = false;

	void GetNewTarget();
	void ClearTarget();

	void LookAtTarget(bool Enabled, float DeltaTime);

	// For determining if an actor (target) is on screen.
	bool ActorInView(AActor* Actor);

	bool ActorOccluded(AActor* Actor);

	void UpdateTargetingBiasLocation(float RayLength = 10000.0f);
	FVector TargetingBiasLocation;

	float TargetingRange = 1000.0f;

	void SortActorsByDistanceToLocation(TArray<AActor*> & Actors, FVector Location);
	bool ActorInRangeOfLocation(AActor* Actor, FVector Location, float Range);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	//APlayerController* PlayerController = Cast<APlayerController>(GetController());

	UCharacterMovementComponent* CharacterMovement = Cast<UCharacterMovementComponent>(GetMovementComponent());

	// Movement and camera input
	void MoveForwardBind(float Axis);
	void MoveRightBind(float Axis);
	void LookUpBind(float Axis);
	void LookRightBind(float Axis);

	// Action1 input
	void Action1PressedBind();
	void Action1ReleasedBind();
	bool Action1Pressed = false;
	bool Action1PressedCache = false;
	bool Action1WasPressed = false;

	// Action2 input
	void Action2PressedBind();
	void Action2ReleasedBind();
	bool Action2Pressed = false;
	bool Action2PressedCache = false;
	bool Action2WasPressed = false;

	// Target input
	void TargetPressedBind();
	void TargetReleasedBind();
public:
	UPROPERTY(BlueprintReadOnly)
		bool TargetPressed = false;
private:
	////////////////////////////////////////////
	//****** Basic Methods *******************//
	////////////////////////////////////////////

	FRotator CharacterFacing;

	// For camera smoothing
	float PitchAmount = 0.0f;
	float YawAmount = 0.0f;

	void Movement(bool Enabled = true);
	void CameraMovement(float DeltaTime, float Smoothing = 0.0f, bool Enabled = true);

	// Rotate character completely to latest movement input direction or rotate character to face target.
	void RotateToInput(float DeltaTime, float Rate = 360.0f, bool Enabled = true, bool Targeting = false);
};
