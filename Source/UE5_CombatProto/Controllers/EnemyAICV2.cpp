// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAICV2.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISense_Touch.h"
#include "Perception/AISense_Damage.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../Characters/EnemyCharacterV2.h"

AEnemyAICV2::AEnemyAICV2()
{
	PrimaryActorTick.bCanEverTick = true;

	SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(FName("Perception Cmp")));
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(FName("Sight Config"));

	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

	SightConfig->SightRadius = 1000.0f;
	SightConfig->LoseSightRadius = 1500.0f;

	SightConfig->PeripheralVisionAngleDegrees = 70.0f;

	GetPerceptionComponent()->ConfigureSense(*SightConfig);

	TeamId = 2;

	InitialReaction = EReactionToEnemy::ERTE_Caution;
}

void AEnemyAICV2::BeginPlay()
{
	Super::BeginPlay();

	if (BehaviorTree)
	{
		RunBehaviorTree(BehaviorTree);
	}

	// Telling the blackboard what the initial reaction to an enemy is.
	GetBlackboardComponent()->SetValueAsEnum(FName("Reaction"), (uint8)InitialReaction);
}

void AEnemyAICV2::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DetermineTarget();

	if (SeesTarget && InRangeOfTarget)
	{
		Cast<AEnemyCharacterV2>(GetPawn())->RotateToActor(Target, DeltaTime);
	}

	if (!SeesTarget && SeesTargetCache)
	{
		LostTarget();
	}
	SeesTargetCache = SeesTarget;
}

ETeamAttitude::Type AEnemyAICV2::GetTeamAttitudeTowards(const AActor& Other) const
{
	// pawn cast condition
	if (const APawn* OtherPawn = Cast<APawn>(&Other))
	{
		// checking if pawn controller implements interface
		if (const IGenericTeamAgentInterface* TeamAgent = Cast<IGenericTeamAgentInterface>(OtherPawn->GetController()))
		{
			FGenericTeamId OtherTeam = TeamAgent->GetGenericTeamId();

			// Rules Start *****************************************
			if (GetGenericTeamId() == 1 && OtherTeam == 1)
			{
				return ETeamAttitude::Friendly;
			}
			else if (GetGenericTeamId() == 2 && OtherTeam == 2)
			{
				return ETeamAttitude::Friendly;
			}
			else if (GetGenericTeamId() == 1 && OtherTeam == 2)
			{
				return ETeamAttitude::Hostile;
			}
			else if (GetGenericTeamId() == 2 && OtherTeam == 1)
			{
				return ETeamAttitude::Hostile;
			}
			else
			{
				return ETeamAttitude::Neutral;
			}
			// Rules End *******************************************
		}
		else 
		{
			return ETeamAttitude::Neutral;
		}
	}
	else 
	{
		return ETeamAttitude::Neutral;
	}
}

FGenericTeamId AEnemyAICV2::GetGenericTeamId() const
{
	return TeamId;
}

void AEnemyAICV2::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	TeamId = NewTeamID;
}

void AEnemyAICV2::SortByDistance(TArray<AActor*>& ActorsToSort, AActor* Actor)
{
	for (int i = 1; i < ActorsToSort.Num(); i++)
	{
		for (int j = 0; j < ActorsToSort.Num() - 1; j++)
		{
			if (FVector::Dist(ActorsToSort[j]->GetActorLocation(), Actor->GetActorLocation()) 
				> 
				FVector::Dist(ActorsToSort[j + 1]->GetActorLocation(), Actor->GetActorLocation()))
			{
				Swap(ActorsToSort[j], ActorsToSort[j + 1]);
			}
		}
	}
}

void AEnemyAICV2::DetermineTarget()
{
	AllActors.Empty();
	FriendlyActors.Empty();
	NeutralActors.Empty();
	HostileActors.Empty();

	Target = nullptr;
	SeesTarget = false;
	InRangeOfTarget = false;

	GetPerceptionComponent()->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), AllActors);

	if (AllActors.Num() > 0)
	{
		for (int i = 0; i < AllActors.Num(); i++)
		{
			if (GetTeamAttitudeTowards(*AllActors[i]) == ETeamAttitude::Friendly)
			{
				FriendlyActors.Emplace(AllActors[i]);
			}
			else if (GetTeamAttitudeTowards(*AllActors[i]) == ETeamAttitude::Neutral)
			{
				NeutralActors.Emplace(AllActors[i]);
			}
			else if (GetTeamAttitudeTowards(*AllActors[i]) == ETeamAttitude::Hostile) {
				HostileActors.Emplace(AllActors[i]);
			}
		}

		//SortByDistance(HostileActors, GetPawn());

		HostileActors.Sort([this](const auto& A, const auto& B)
			{
				auto pawnLocation = GetPawn()->GetActorLocation();
				return FVector::Dist(A.GetActorLocation(), pawnLocation) < FVector::Dist(B.GetActorLocation(), pawnLocation);
			}
		);

		if (HostileActors.Num() > 0)
		{
			//DEBUG_MSG(90, 0.02f, Orange, HostileActors[0]->GetName());
			Target = HostileActors[0];
			SeesTarget = true;

			GetBlackboardComponent()->SetValueAsVector(FName("LastKnownTargetLoc"), Target->GetActorLocation());

			if (FVector::Dist(GetPawn()->GetActorLocation(), Target->GetActorLocation()) <= AttackRange)
			{
				InRangeOfTarget = true;
			}
		}
	}

	/* Debug */ if (GEngine) GEngine->AddOnScreenDebugMessage(100, 0.1f, FColor::Magenta, FString::FromInt(HostileActors.Num()));

	GetBlackboardComponent()->SetValueAsObject(FName("TargetActor"), Target);
	GetBlackboardComponent()->SetValueAsBool(FName("SeesTarget"), SeesTarget);
	GetBlackboardComponent()->SetValueAsBool(FName("InRangeOfTarget"), InRangeOfTarget);
}

void AEnemyAICV2::LostTarget()
{
	DEBUG_MSG(-1, 4, Orange, TEXT("Lost target!"));
	// start searching for target upon losing it.
	GetBlackboardComponent()->SetValueAsBool(FName("ShouldSearch"), true);
}

