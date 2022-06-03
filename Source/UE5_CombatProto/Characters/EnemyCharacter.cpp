// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"
#include "../AICtrlBase.h"

AEnemyCharacter::AEnemyCharacter()
{
	
}

float AEnemyCharacter::GetMovementInputStrength_Implementation()
{
	float result = this->GetVelocity().Length() / (600.0f*1.2f); // Max Walk speed * Mesh Scale.
	return result;
}

void AEnemyCharacter::Attack()
{
	if (!IsDead && !AttackCoolingDown)
	{
		AttackCoolingDown = true;
		if (FastAttack) PlayAnimMontage(FastAttack);
	}
}

float AEnemyCharacter::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Death();
	return 0.0f;
}

void AEnemyCharacter::UniqueDeath()
{
	Cast<ABaseAICtrl>(GetController())->DisableAI();
}
