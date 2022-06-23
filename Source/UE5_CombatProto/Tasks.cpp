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

// DOESN'T WORK
EBTNodeResult::Type UGetActorsLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AActor* Tar_Actor;
	UObject* O = OwnerComp.GetBlackboardComponent()->GetValue<UBlackboardKeyType_Object>(BlackboardKey.GetSelectedKeyID());
	Tar_Actor = Cast<AActor>(O);
	/* Debug */if (GEngine && Tar_Actor)
	{
		GEngine->AddOnScreenDebugMessage(70, 1, FColor::Purple, Tar_Actor->GetName());
		GEngine->AddOnScreenDebugMessage(71, 1, FColor::Purple, Tar_Actor->GetActorLocation().ToString());
	}

	if(Tar_Actor) OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Vector>(Location.GetSelectedKeyID(), Tar_Actor->GetActorLocation());
	return EBTNodeResult::Succeeded;
}
