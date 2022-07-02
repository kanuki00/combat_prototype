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
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};

UCLASS()
class UE5_COMBATPROTO_API UInterruptedMoveTo : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	// Moving the AI Character to a location continuously
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};

UCLASS()
class UE5_COMBATPROTO_API UStopMoving : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	// Called in behavior tree to stop any navmesh movement.
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};

UCLASS()
class UE5_COMBATPROTO_API UGetActorLocation : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

	void InitializeFromAsset(UBehaviorTree& Asset) override;

	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	UPROPERTY(EditAnywhere, Category = Blackboard)
		struct FBlackboardKeySelector ActorKey;
	UPROPERTY(EditAnywhere, Category = Blackboard)
		struct FBlackboardKeySelector LocationKey;
};

UCLASS()
class UE5_COMBATPROTO_API UGetRandomLocationInRange : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

public:
	UPROPERTY(EditAnywhere, Category = "Radius")
		float Radius = 0.0f;

};

UCLASS()
class UE5_COMBATPROTO_API UAttack : public UBTTaskNode
{
	GENERATED_BODY()

	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};

UCLASS()
class UE5_COMBATPROTO_API USetBBEntryBool : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	public:
		UPROPERTY(EditAnywhere)
			bool NewValue = false;
};

UCLASS()
class UE5_COMBATPROTO_API UFindEscapeLocation : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

	void InitializeFromAsset(UBehaviorTree& Asset) override;
	
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
protected:
	UPROPERTY(EditAnywhere, Category = Blackboard)
		struct FBlackboardKeySelector ActorToEscFromKey;
	UPROPERTY(EditAnywhere, Category = Blackboard)
		struct FBlackboardKeySelector LocationKey;
};