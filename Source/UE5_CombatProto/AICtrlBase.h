// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AICtrlBase.generated.h"

/**
 * 
 */
UCLASS()
class UE5_COMBATPROTO_API AAICtrlBase : public AAIController
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Asset References")
		UBlackboardData* UsedBlackboard = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Asset References")
		UBehaviorTree* UsedBehaviorTree = nullptr;

protected:
	virtual void BeginPlay() override;
};
