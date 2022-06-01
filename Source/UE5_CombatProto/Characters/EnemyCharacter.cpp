// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"

AEnemyCharacter::AEnemyCharacter()
{
	
}

float AEnemyCharacter::GetMovementInputStrength_Implementation()
{
	float result = this->GetVelocity().Length() / (600.0f*1.2f); // Max Walk speed * Mesh Scale.
	return result;
}
