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

	UPROPERTY(EditAnywhere, Category = "Team")
		int Team = 0;

	void RotateToActor(AActor* Actor, float DeltaTime, float Speed = 480.0f);
protected:
	void UniqueTakeDamage() override;
	void UniqueDeath() override;

	
private:

};
