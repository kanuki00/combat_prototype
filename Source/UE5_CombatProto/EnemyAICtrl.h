// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AICtrlBase.h"
#include "EnemyAICtrl.generated.h"

/**
 * 
 */
UCLASS()
class UE5_COMBATPROTO_API AEnemyAICtrl : public ABaseAICtrl
{
	GENERATED_BODY()

public:
	AEnemyAICtrl();

protected:
	void Tick(float DeltaTime);

	bool SeesPlayer = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float AttackRange = 200.0f;
	UPROPERTY(BlueprintReadOnly)
		bool InRangeOfPlayer;
};
