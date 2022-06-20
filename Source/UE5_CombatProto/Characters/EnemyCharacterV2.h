// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacterV2.h"
#include "EnemyCharacterV2.generated.h"

/**
 * 
 */
UCLASS()
class UE5_COMBATPROTO_API AEnemyCharacterV2 : public ABaseCharacterV2
{
	GENERATED_BODY()

public:
	// *********************************
	// Constructor
	// *********************************
	AEnemyCharacterV2();

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
protected:
private:

};