// Fill out your copyright notice in the Description page of Project Settings.


#include "AICtrlBase.h"

ABaseAICtrl::ABaseAICtrl()
{
	PrimaryActorTick.bCanEverTick = true;

	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(FName("Sight Config"));
	SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(FName("Perception")));

	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;

	SightConfig->SightRadius = 800.0f;
	SightConfig->LoseSightRadius = 900.0f;

	GetPerceptionComponent()->ConfigureSense(*SightConfig);
}

void ABaseAICtrl::BeginPlay()
{
	Super::BeginPlay();
	RunBehaviorTree(UsedBehaviorTree);
}

void ABaseAICtrl::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SensedActors.Empty();
	GetPerceptionComponent()->GetPerceivedActors(UAISense_Sight::StaticClass(), SensedActors);
	if (GEngine && false && SensedActors.Num() > 0) GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Orange, SensedActors[0]->GetName());
}
