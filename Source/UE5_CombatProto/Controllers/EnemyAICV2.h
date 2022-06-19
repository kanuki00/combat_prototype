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

protected:
	UAISenseConfig_Sight* SightConfig;

private:
	FGenericTeamId TeamId;
};
