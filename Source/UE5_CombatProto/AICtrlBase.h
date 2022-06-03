// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISenseConfig_Sight.h"
#include "BrainComponent.h"
#include "AICtrlBase.generated.h"

UCLASS()
class UE5_COMBATPROTO_API ABaseAICtrl : public AAIController
{
	GENERATED_BODY()

public:
	ABaseAICtrl();

	UFUNCTION()
		void DisableAI();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Asset References")
		UBehaviorTree* UsedBehaviorTree = nullptr;

	UPROPERTY()
		class UAISenseConfig_Sight* SightConfig;

protected:
	virtual void BeginPlay() override;

	UPROPERTY()
		TArray<AActor*> SensedActors;

	void Tick(float DeltaTime);
};
