// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacterV2.h"
#include "../BaseWidget.h"
#include "../Controllers/EnemyAICV2.h"
#include "EnemyCharacterV2.generated.h"

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

	UPROPERTY(EditAnywhere, Category = "AI")
		int Team = 0;

	UPROPERTY(EditAnywhere, Category = "AI")
		EReactionToEnemy PawnInitialReaction = EReactionToEnemy::ERTE_Attack;

	void RotateToActor(AActor* Actor, float DeltaTime, float Speed = 480.0f);
	bool CanStartAttack = true;
	void StartAttack();

protected:
	void UniqueTakeDamage(AActor* DamageCauser) override;
	void UniqueDeath() override;

	AEnemyAICV2* AIController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		TArray<UAnimMontage*> AttackAnimations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting")
		UBaseWidget* TargetWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting")
		UBaseWidget* SuggestWidget;
private:

};
