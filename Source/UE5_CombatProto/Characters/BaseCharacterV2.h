// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BaseCharacterV2.generated.h"

#ifndef DEBUG_MSG
#define DEBUG_MSG(message) if(GEngine)GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, message);
#endif

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
	// Character functionality 
	// *********************************
public:
	// Taking damage. Overridden from AActor.
	float TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController* EventInstigator, AActor * DamageCauser) override;

	// Is Character stunned? If true, character shouldn't be able to move or rotate.
	bool IsStunned = false;
	//
	bool IsDead = false;
	// 
	bool CanMove = true;

	UPROPERTY(BlueprintReadOnly)
		float DefaultMaxWalkSpeed;

	UPROPERTY(BlueprintReadOnly)
		bool StrafeEnabled = false;

	// Simple attack method. Used for development. Does a capsule trace in front of 
	// character and applies damage to any characters that are in the trace.
	void SimpleAttack(float Damage);

	// Character's desired movement input. 
	UPROPERTY(BlueprintReadOnly)
		FVector MovementInput;

	bool CanApplyDamage = false;

	void WeaponDamage(TArray<AActor*> Overlapped);

	float GetHealth();
	float GetMaxHealth();

	bool CountSecsSinceLastDamage = false;
	float SecondsSinceLastDamage = 1000000.0f;

	float WeaponDamageMultiplier = 1.0f;

protected:
	// Character implements something unique to do upon taking damage.
	virtual void UniqueTakeDamage(AActor* DamageCauser);

	// Method to flip a bool from a defined value after a certain delay. 
	// Timer and Delay variables must be created for each bool that gets this functionality.
	// Call in tick.
	void FlipBoolAfterDelay(bool& Bool, bool ValueToFlipFrom, float& Timer, float Delay, float DeltaTime);

	// Death is called when characters health falls to 0 upon taking damage.
	void Death();

	// Character implements something unique to do upon death.
	virtual void UniqueDeath();

	bool SimpleAttackCoolingDown = false;
	float S_A_CooldownTimer = 0.0f;
	float S_A_Cooldown = 0.8f;

	UCharacterMovementComponent* CharMovComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		// If charactershould spawn with weapon in hand.
		bool SpawnWithWeapon = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		// Determine what weapon character is holding
		TSubclassOf<AActor> WeaponClass;

	// Pointer that stores whichever weapon that the character is holding.
	AActor* Weapon = nullptr;

	UPROPERTY(BlueprintReadOnly)
		bool IsArmed = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		// Characters maximum health.
		float MaxHealth = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		// Characters health.
		float Health = 100.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Health")
		// Can character take damage? Turn this to false if you want character to be invincible.
		bool CanTakeDamage = true;

private:

	// Cooldown for damage taking, so that damage cant be applied unnececarily fast.
	bool TakeDamageCoolingDown = false;
	float TakeDamageCooldownTimer = 0.0f;
	float TakeDamageCooldown = 0.333f;


};
