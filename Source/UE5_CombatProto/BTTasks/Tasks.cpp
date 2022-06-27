// Fill out your copyright notice in the Description page of Project Settings.


#include "Tasks.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "AIController.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"
#include "../Characters/EnemyCharacterV2.h"


EBTNodeResult::Type UGetPlayerLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FVector PlayerLoc = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->GetActorLocation();
	OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Vector>(BlackboardKey.GetSelectedKeyID(), PlayerLoc);
	return EBTNodeResult::Succeeded;
}

EBTNodeResult::Type UInterruptedMoveTo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIC = OwnerComp.GetAIOwner();
	FVector MoveLocation = OwnerComp.GetBlackboardComponent()->GetValue<UBlackboardKeyType_Vector>(BlackboardKey.GetSelectedKeyID());
	AIC->MoveToLocation(MoveLocation, 50.0f);

	return EBTNodeResult::Succeeded;
}

EBTNodeResult::Type UStopMoving::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIC = OwnerComp.GetAIOwner();
	AIC->StopMovement();
	return EBTNodeResult::Succeeded;
}

void UGetActorLocation::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	UBlackboardData* BBAsset = GetBlackboardAsset();
	if (BBAsset)
	{
		ActorKey.ResolveSelectedKey(*BBAsset);
		LocationKey.ResolveSelectedKey(*BBAsset);
	}
	else
	{
		UE_LOG(LogBehaviorTree, Warning, TEXT("Can't initialize task: %s, make sure that behavior tree specifies blackboard asset!"), *GetName());
	}
}

EBTNodeResult::Type UGetActorLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AActor* Tar_Actor;
	FVector Tar_Location;

	Tar_Actor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValue<UBlackboardKeyType_Object>(ActorKey.GetSelectedKeyID()));

	if (Tar_Actor) Tar_Location = Tar_Actor->GetActorLocation();

	OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Vector>(LocationKey.GetSelectedKeyID(), Tar_Location);

	return EBTNodeResult::Succeeded;
}

EBTNodeResult::Type UGetRandomLocationInRange::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	//FVector RandomLoc = UNavigationSystemV1::GetRandomReachablePointInRadius(GetWorld(), OwnerComp.GetAIOwner()->GetPawn()->GetActorLocation(), Radius);
	//OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Vector>(BlackboardKey.GetSelectedKeyID(), RandomLoc);
	return EBTNodeResult::Succeeded;
}

EBTNodeResult::Type UAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AEnemyCharacterV2* ControlledEnemy = Cast<AEnemyCharacterV2>(OwnerComp.GetAIOwner()->GetPawn());
	ControlledEnemy->SimpleAttack(25.0f);
	return EBTNodeResult::Type();
}

EBTNodeResult::Type USetBBEntryBool::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Bool>(BlackboardKey.GetSelectedKeyID(), NewValue);
	return EBTNodeResult::Succeeded;
}
