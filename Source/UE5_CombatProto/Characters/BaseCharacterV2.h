// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacterV2.generated.h"

UCLASS()
class UE5_COMBATPROTO_API ABaseCharacterV2 : public ACharacter
{
	GENERATED_BODY()

public:
	// *********************************
	// Constructor
	// *********************************
	ABaseCharacterV2();

protected:
	// *********************************
	// Begin Play
	// *********************************
	virtual void BeginPlay() override;

public:
	// *********************************
	// Tick
	// *********************************
	virtual void Tick(float DeltaTime) override;

	// *********************************
	// Bind
	// *********************************
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// *********************************
	// Character functionality 
	// *********************************
public:
	// Taking damage. Overridden from AActor.
	float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor * DamageCauser) override;

protected:
	// Is Character stunned? If true, character shouldn't be able to move or rotate.
	bool IsStunned = false;
	//
	bool IsDead = false;
	// 
	bool CanMove = true;
	// Character implements something unique to do upon taking damage.
	virtual void UniqueTakeDamage();

	// Method to flip a bool from a defined value after a certain delay. 
	// Timer and Delay variables must be created for each bool that gets this functionality.
	// Call in tick.
	void FlipBoolAfterDelay(bool& Bool, bool ValueToFlipFrom, float& Timer, float& Delay, float DeltaTime);

	// Death is called when characters health falls to 0 upon taking damage.
	void Death();

	// Character implements something unique to do upon death.
	virtual void UniqueDeath();

private:

	UPROPERTY()
		// Characters maximum health.
		float MaxHealth = 100.0f;
	UPROPERTY()
		// Characters health.
		float Health = 100.0f;
	
	UPROPERTY()
		// Can character take damage? Turn this to false if you want character to be invincible.
		bool CanTakeDamage = true;
	
	// Cooldown for damage taking, so that damage cant be applied unnececarily fast.
	bool TakeDamageCoolingDown = false;
	float TakeDamageCooldownTimer = 0.0f;
	float TakeDamageCooldown = 0.333f;
};