// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GenericTeamAgentInterface.h"
#include "CustomPlayerCtrl.generated.h"

/**
 * 
 */
UCLASS()
class UE5_COMBATPROTO_API ACustomPlayerCtrl : public APlayerController, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

	FGenericTeamId TeamId;

public:
	ACustomPlayerCtrl();
	FGenericTeamId GetGenericTeamId() const;
};
