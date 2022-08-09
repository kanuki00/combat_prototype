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

#include "../GameMacros.h"


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
	if (!ControlledEnemy) return EBTNodeResult::Failed;
	if (PerformDebugAttack)
	{
		ControlledEnemy->SimpleAttack(25.0f);
	}
	else
	{
		ControlledEnemy->StartAttack();
	}
	return EBTNodeResult::Succeeded;
}

EBTNodeResult::Type USetBBEntryBool::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Bool>(BlackboardKey.GetSelectedKeyID(), NewValue);
	return EBTNodeResult::Succeeded;
}

UFindEscapeLocation::UFindEscapeLocation()
{
	NodeName = "Find Escape Location";
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
/* Calculating where enemy should escape to */
EBTNodeResult::Type UFindEscapeLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	float ShortestEscDist = 300.0f;
	float EscDist = 800.0f;
	EDrawDebugTrace::Type Debugmode = EDrawDebugTrace::None;

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
	UKismetSystemLibrary::LineTraceSingle(GetWorld(), PawnLoc, PawnLoc + Spread.RotateVector(EscDirection * EscDist), ETraceTypeQuery::TraceTypeQuery1, true, I, Debugmode, ForwardTraceHitResult, false);
	
	if (ForwardTraceHitResult.bBlockingHit)
	{
		// Trace another line along the tangent of first hit if the first trace distance is too short.
		if ((ForwardTraceHitResult.Location - PawnLoc).Length() < ShortestEscDist)
		{
			FVector SecondaryTracesStart = ForwardTraceHitResult.Location + ForwardTraceHitResult.Normal * 40.0f;
			FHitResult RightTraceHitResult;
			FHitResult LeftTraceHitResult;
			// Trace to the right of forward trace location
			UKismetSystemLibrary::LineTraceSingle(GetWorld(), SecondaryTracesStart, 
			SecondaryTracesStart + FRotator(0.0f, -90.0f, 0.0f).RotateVector(ForwardTraceHitResult.Normal * EscDist), 
			ETraceTypeQuery::TraceTypeQuery1, true, I, Debugmode, RightTraceHitResult, false, FLinearColor::Yellow);

			// Trace to the left of forward trace location
			UKismetSystemLibrary::LineTraceSingle(GetWorld(), SecondaryTracesStart, 
			SecondaryTracesStart + FRotator(0.0f, 90.0f, 0.0f).RotateVector(ForwardTraceHitResult.Normal * EscDist), 
			ETraceTypeQuery::TraceTypeQuery1, true, I, Debugmode, LeftTraceHitResult, false, FLinearColor::Blue);

			// Choose by likeness
			if (FVector::DotProduct((ForwardTraceHitResult.TraceEnd - ForwardTraceHitResult.TraceStart).GetSafeNormal(), (FRotator(0.0f, -90.0f, 0.0f).RotateVector(ForwardTraceHitResult.Normal * EscDist)).GetSafeNormal()) > 0.0f)
			{
				if (RightTraceHitResult.bBlockingHit)
				{
					if ((RightTraceHitResult.Location - PawnLoc).Length() < ShortestEscDist)
					{
						NewEscapeLoc = LeftTraceHitResult.bBlockingHit ? LeftTraceHitResult.Location : LeftTraceHitResult.TraceEnd;
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
				if (LeftTraceHitResult.bBlockingHit)
				{
					if ((LeftTraceHitResult.Location - PawnLoc).Length() < ShortestEscDist)
					{
						NewEscapeLoc = RightTraceHitResult.bBlockingHit ? RightTraceHitResult.Location : RightTraceHitResult.TraceEnd;
					}
					else
					{
						NewEscapeLoc = LeftTraceHitResult.Location;
					}
				}
				else
				{
					NewEscapeLoc = LeftTraceHitResult.TraceEnd;
				}
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

USetControllerFocus::USetControllerFocus()
{
	NodeName = "Set Controller Focus";
}

EBTNodeResult::Type USetControllerFocus::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	//if (BlackboardKey == NULL) return EBTNodeResult::Failed;

	FVector Location;
	AActor* PossibleActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValue<UBlackboardKeyType_Object>(BlackboardKey.GetSelectedKeyID()));

	if (PossibleActor) // Testing if key is an actor. if true, location is actors location
	{
		Location = PossibleActor->GetActorLocation();
	}
	else // if it's not an actor, then it must be a vector.
	{
		Location = OwnerComp.GetBlackboardComponent()->GetValue<UBlackboardKeyType_Vector>(BlackboardKey.GetSelectedKeyID());
	}

	OwnerComp.GetAIOwner()->SetFocalPoint(Location, EAIFocusPriority::Gameplay);
	return EBTNodeResult::Succeeded;
}

/********** Find Cautious Location ***********/

UFindCautiousLocation::UFindCautiousLocation()
{
	NodeName = "Find Cautious Location";
}

void UFindCautiousLocation::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	UBlackboardData* BBAsset = GetBlackboardAsset();
	if (BBAsset)
	{
		ActorToBeCautiousOfKey.ResolveSelectedKey(*BBAsset);
		MoveLocationKey.ResolveSelectedKey(*BBAsset);
	}
	else
	{
		UE_LOG(LogBehaviorTree, Warning, TEXT("Can't initialize task: %s, make sure that behavior tree specifies blackboard asset!"), *GetName());
	}
}

EBTNodeResult::Type UFindCautiousLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AActor* Actor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValue<UBlackboardKeyType_Object>(ActorToBeCautiousOfKey.GetSelectedKeyID()));
	AEnemyCharacterV2* ControlledPawn = Cast<AEnemyCharacterV2>(OwnerComp.GetAIOwner()->GetPawn());

	// Telling pawn that it can start attacking again.
	ControlledPawn->CanStartAttack = true;

	float Distance = FVector::Distance(Actor->GetActorLocation(), ControlledPawn->GetActorLocation());

	float MaxMove = SafeDistanceMax - SafeDistanceMin;

	if (Distance < SafeDistanceMin)
	{
		//DEBUG_MESSAGE(-1, 1, FColor::Cyan, TEXT("Too close!"));
		FVector Away = ControlledPawn->GetActorLocation() - Actor->GetActorLocation();
		Away = Away.GetSafeNormal();
		OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Vector>(MoveLocationKey.GetSelectedKeyID(), 
			ControlledPawn->GetActorLocation() + Away * MaxMove);
	}
	else if (Distance > SafeDistanceMax)
	{
		//DEBUG_MESSAGE(-1, 1, FColor::Cyan, TEXT("Too far!"));
		FVector Towards = Actor->GetActorLocation() - ControlledPawn->GetActorLocation();
		Towards = Towards.GetSafeNormal();
		OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Vector>(MoveLocationKey.GetSelectedKeyID(),
			ControlledPawn->GetActorLocation() + Towards * MaxMove);
	}
	// if in safe distance from target, pawn will circle target.
	else 
	{
		ControlledPawn->RandomLeftRight();

		if (ControlledPawn->Left)
		{
			FVector ToLeft = -ControlledPawn->GetActorRightVector();
			OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Vector>(MoveLocationKey.GetSelectedKeyID(),
				ControlledPawn->GetActorLocation() + ToLeft * MaxMove);
		}
		else
		{
			FVector ToRight = ControlledPawn->GetActorRightVector();
			OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Vector>(MoveLocationKey.GetSelectedKeyID(),
				ControlledPawn->GetActorLocation() + ToRight * MaxMove);
		}
		//DEBUG_MESSAGE(-1, 1, FColor::Cyan, TEXT("Just right."));
	}

	return EBTNodeResult::Succeeded;
}
