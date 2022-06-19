// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAICV2.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISense_Touch.h"
#include "Perception/AISense_Damage.h"

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
}

void AEnemyAICV2::BeginPlay()
{
	Super::BeginPlay();

	if (BehaviorTree)
	{
		RunBehaviorTree(BehaviorTree);
	}
}

void AEnemyAICV2::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TArray<AActor*> SensedActors;
	GetPerceptionComponent()->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), SensedActors);

	if (SensedActors.Num() > 0)
	{
		DEBUG_MSG(1, 0.2f, Cyan, TEXT("Sensed"));

		ETeamAttitude::Type ZAttitude = GetTeamAttitudeTowards(*SensedActors[0]);
		switch(ZAttitude)
		{
			case ETeamAttitude::Neutral:
				DEBUG_MSG(2, 0.2f, Orange, TEXT("Neutral"));
				break;
			case ETeamAttitude::Friendly:
				DEBUG_MSG(2, 0.2f, Orange, TEXT("Friendly"));
				break;
			case ETeamAttitude::Hostile:
				DEBUG_MSG(2, 0.2f, Orange, TEXT("Hostile"));
				break;
		}
	}
}

ETeamAttitude::Type AEnemyAICV2::GetTeamAttitudeTowards(const AActor& Other) const
{
	return ETeamAttitude::Friendly;
}

