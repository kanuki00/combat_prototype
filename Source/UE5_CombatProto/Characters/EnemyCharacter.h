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
	bool AttackCoolingDown = false;

protected:
	//virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	void Tick(float DeltaTime);

	bool CanRotateToPlayer = false;

	virtual void UniqueDeath() override;

	void RotateToPlayer(float DeltaTime);

	virtual void SetCharacterRotationEnabled(bool NewRotate = true) override;
};
