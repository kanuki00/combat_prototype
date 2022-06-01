// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
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
};