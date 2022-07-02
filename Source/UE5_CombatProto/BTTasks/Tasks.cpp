// Fill out your copyright notice in the Description page of Project Settings.


#include "Tasks.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
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

void UFindEscapeLocation::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	UBlackboardData* BBAsset = GetBlackboardAsset();
	if (BBAsset)
	{
		ActorToEscFromKey.ResolveSelectedKey(*BBAsset);
		LocationKey.ResolveSelectedKey(*BBAsset);
	}
	else
	{
		UE_LOG(LogBehaviorTree, Warning, TEXT("Can't initialize task: %s, make sure that behavior tree specifies blackboard asset!"), *GetName());
	}
}

EBTNodeResult::Type UFindEscapeLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	float ShortestEscDist = 300.0f;
	FVector NewEscapeLoc;
	AActor* Actor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValue<UBlackboardKeyType_Object>(ActorToEscFromKey.GetSelectedKeyID()));
	if(Actor == nullptr) return EBTNodeResult::Succeeded;
	FVector EscDirection = OwnerComp.GetAIOwner()->GetPawn()->GetActorLocation() - Actor->GetActorLocation();
	FVector PawnLoc = OwnerComp.GetAIOwner()->GetPawn()->GetActorLocation();
	EscDirection = EscDirection.GetSafeNormal();
	FRotator Spread = FRotator(0.0f, UKismetMathLibrary::RandomFloat()*10-5, 0.0f);
	TArray<AActor*> I; I.Init(OwnerComp.GetAIOwner()->GetPawn(), 1);
	FHitResult ForwardTraceHitResult;
	// First trace directly away from target actor.
	UKismetSystemLibrary::LineTraceSingle(GetWorld(), PawnLoc, PawnLoc + Spread.RotateVector(EscDirection * 1000), ETraceTypeQuery::TraceTypeQuery1, true, I, EDrawDebugTrace::ForDuration, ForwardTraceHitResult, false);
	/* Debug *///if (GEngine && HitResult.GetActor()) GEngine->AddOnScreenDebugMessage(10, 0.2f, FColor::Orange, HitResult.GetActor()->GetName());
	/* Debug *///if (GEngine) GEngine->AddOnScreenDebugMessage(11, 0.2f, FColor::Blue, I[0]->GetName());
	if (ForwardTraceHitResult.bBlockingHit)
	{
		if ((ForwardTraceHitResult.Location - PawnLoc).Length() < ShortestEscDist)
		{
			FHitResult RightTraceHitResult;
			// Second trace to the right of the enemy. Traced if first trace distance is too short.
			UKismetSystemLibrary::LineTraceSingle(GetWorld(), PawnLoc, PawnLoc + FRotator(0.0f, -90.0f, 0.0f).RotateVector(Spread.RotateVector(EscDirection * 1000)), ETraceTypeQuery::TraceTypeQuery1, true, I, EDrawDebugTrace::ForDuration, RightTraceHitResult, false, FLinearColor::Yellow);
			if (RightTraceHitResult.bBlockingHit)
			{
				if ((RightTraceHitResult.Location - PawnLoc).Length() < ShortestEscDist)
				{
					FHitResult LeftTraceHitResult;
					// Third trace to the enemy's left.
					UKismetSystemLibrary::LineTraceSingle(GetWorld(), PawnLoc, PawnLoc + FRotator(0.0f, 90.0f, 0.0f).RotateVector(Spread.RotateVector(EscDirection * 1000)), ETraceTypeQuery::TraceTypeQuery1, true, I, EDrawDebugTrace::ForDuration, LeftTraceHitResult, false, FLinearColor::Blue);
					if (LeftTraceHitResult.bBlockingHit)
					{
						NewEscapeLoc = LeftTraceHitResult.Location;
					}
					else
					{
						NewEscapeLoc = LeftTraceHitResult.TraceEnd;
					}
				}
				else 
				{
					NewEscapeLoc = RightTraceHitResult.Location;
				}
			}
			else
			{
				NewEscapeLoc = RightTraceHitResult.TraceEnd;
			}
		}
		else
		{
			NewEscapeLoc = ForwardTraceHitResult.Location;
		}
	}
	else {
		NewEscapeLoc = ForwardTraceHitResult.TraceEnd;
	}
	OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Vector>(LocationKey.GetSelectedKeyID(), NewEscapeLoc);
	return EBTNodeResult::Succeeded;
}
