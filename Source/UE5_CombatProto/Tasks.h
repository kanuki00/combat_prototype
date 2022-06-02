// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "Tasks.generated.h"

UCLASS()
class UE5_COMBATPROTO_API UGetPlayerLocation : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	// Getting the player's location
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};

UCLASS()
class UE5_COMBATPROTO_API UMoveToLocation : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	// Moving the AI Character to a location continuously
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};

UCLASS()
class UE5_COMBATPROTO_API UStopMoving : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	// Called in behavior tree to stop any navmesh movement.
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
