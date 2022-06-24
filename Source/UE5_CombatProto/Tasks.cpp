// Fill out your copyright notice in the Description page of Project Settings.


#include "Tasks.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "AIController.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"


EBTNodeResult::Type UGetPlayerLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FVector PlayerLoc = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->GetActorLocation();
	OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Vector>(BlackboardKey.GetSelectedKeyID(), PlayerLoc);
	return EBTNodeResult::Succeeded;
}

EBTNodeResult::Type UMoveToLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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
