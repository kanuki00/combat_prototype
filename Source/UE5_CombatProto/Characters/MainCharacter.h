// Copyright Kalevi Toivanen 2022.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "MainCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;

// Main character class
UCLASS()
class UE5_COMBATPROTO_API APlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	////////////////////////////////////////////
	//************* DEBUG ********************//
	////////////////////////////////////////////
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool MainCharacterDebug = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int MainCharacterDebugLevel = -1;

	////////////////////////////////////////////
	//************* Constructor **************//
	////////////////////////////////////////////
	APlayerCharacter();

	// Component pointers
	UPROPERTY(EditAnywhere)
		UCameraComponent* Camera;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USpringArmComponent* CameraBoom;

protected:
	////////////////////////////////////////////
	//************* Begin play ***************//
	////////////////////////////////////////////
	virtual void BeginPlay() override;

public:
	////////////////////////////////////////////
	//************** Tick ********************//
	////////////////////////////////////////////
	virtual void Tick(float DeltaTime) override;

private:
	////////////////////////////////////////////
	//****** Basic Methods *******************//
	////////////////////////////////////////////
	UPROPERTY()
		FRotator CharacterFacing;

	// For camera smoothing
	UPROPERTY()
		float PitchAmount = 0.0f;
	UPROPERTY()
		float YawAmount = 0.0f;

	UFUNCTION()
		void Movement(bool Enabled = true);
	UFUNCTION()
		void CameraMovement(float DeltaTime, float Smoothing = 0.0f, bool Enabled = true);

	// Rotate character completely to latest movement input direction or rotate character to face target.
	UFUNCTION()
		void RotateToInput(float DeltaTime, float Rate = 360.0f, bool Enabled = true, bool Targeting = false);

protected:
	virtual void UniqueDeath() override;

public:
	////////////////////////////////////////////
	//************** Bind ********************//
	////////////////////////////////////////////
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

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

private:
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

	// Dodge/sprint input bind
	void Input3PressedBind();
	void Input3ReleasedBind();
	float Input3PressTimer = 0.0f;
	bool Input3Pressed = false;
	float Input3TapThresh = 0.2f;
	void Input3Tapped();
	bool Input3Held = false;

	// Target input
	void TargetPressedBind();
	void TargetReleasedBind();

public:
	UPROPERTY(BlueprintReadOnly)
		bool TargetPressed = false;

protected:
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
		AActor* Target = nullptr;

	// Pointer to target suggested by targeting logic. Used to draw suggestion widget.
	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
		AActor* SuggestedTarget = nullptr;

	UPROPERTY(BlueprintReadOnly) // Visible to blueprint so that anim bp knows when to strafe.
		bool IsTargeting = false;
private:
	void GetNewTarget();
	void ClearTarget();

	void LookAtTarget(bool Enabled, float DeltaTime);

	// For determining if an actor (target) is on screen.
	bool ActorInView(AActor* Actor);

	bool ActorOccluded(AActor* Actor);

	void UpdateTargetingBiasLocation(float RayLength = 10000.0f);
	FVector TargetingBiasLocation;
	FVector2D ScreenTargetingBiasLocation; // Unused for now, I want to try an approach of compating target's screen position to this bias location for target determining in the future.

	float TargetingRange = 2500.0f;

	void SortActorsByDistanceToLocation(TArray<AActor*>& Actors, FVector Location);
	bool ActorInRangeOfLocation(AActor* Actor, FVector Location, float Range);

private:
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

public:
	void StartFastAttack();
	void StartStrongAttack();

	void CheckAttackPressed();
	
	void ContinueAttack();

	void TransToStrongAttack();
	void TransToFastAttack();

	void EndAttack();

protected:
	////////////////////////////////////////////
	//****** Interface implementations *******//
	////////////////////////////////////////////

	// Implementation for character animation interface
	float GetMovementInputStrength_Implementation();
	FVector GetMovementInputDirection_Implementation();

	// Implementation for combat interface.
	virtual void WeaponHit(AActor* HitActor) override;
};
