// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "EnemyCharacter.generated.h"

/**
 * 
 */
UCLASS()
class UE5_COMBATPROTO_API AEnemyCharacter : public ABaseCharacter
{
	GENERATED_BODY()
	
public:
	AEnemyCharacter();

	float GetMovementInputStrength_Implementation();

	void Attack();
	bool AttackCoolingDown = false; // this cooldown is for actually starting the attack animation.
	bool AttackEndCooldown = false; // this cooldown starts at end of attack animation so that there's a window for the player to attack the enemy.
									// both are getting set in GeneralNotifies -> UResetEnemyAtkCooldownNotify.

protected:
	//virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	void Tick(float DeltaTime);

	bool CanRotateToPlayer = false;

	void UniqueDeath() override;

	void UniqueTakeDamage() override;

	void RotateToPlayer(float DeltaTime);

	virtual void SetCharacterRotationEnabled(bool NewRotate = true) override;


	float AttackEndCooldownTimer = 0.0f;
	float AttackEndCooldownLength = 1.0f;
};
