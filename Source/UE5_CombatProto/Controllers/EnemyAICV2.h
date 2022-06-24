// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAICV2.generated.h"

#define DEBUG_MSG(key, duration, color, message) if(GEngine) GEngine->AddOnScreenDebugMessage(key, duration, FColor::color, message);

class UAISenseConfig_Sight;

UCLASS()
class UE5_COMBATPROTO_API AEnemyAICV2 : public AAIController
{
	GENERATED_BODY()

public:
	// *********************************
	// Constructor
	// *********************************
	AEnemyAICV2();

protected:
	// *********************************
	// Begin Play
	// *********************************
	virtual void BeginPlay() override;

public:
	// *********************************
	// Tick
	// *********************************
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditAnywhere)
		UBehaviorTree* BehaviorTree = nullptr;

	ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;

	FGenericTeamId GetGenericTeamId() const;

	void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;

	void Shout()
	{
		DEBUG_MSG(10, 4, Yellow, TEXT("EnemyAI"));
		DEBUG_MSG(10, 4, Yellow, FString::FromInt(GetGenericTeamId()));
	}

protected:
	UAISenseConfig_Sight* SightConfig;

	UPROPERTY()
		AActor* Target = nullptr;
	UPROPERTY()
		bool SeesTarget = false;
	UPROPERTY()
		bool InRangeOfTarget = false;

	float AttackRange = 150.0f;

private:
	FGenericTeamId TeamId;

	TArray<AActor*> AllActors;
	TArray<AActor*> FriendlyActors;
	TArray<AActor*> NeutralActors;
	TArray<AActor*> HostileActors;

	void SortByDistance(TArray<AActor*> & ActorsToSort, AActor* Actor);
};
