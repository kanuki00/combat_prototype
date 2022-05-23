// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MainCharacter.h"
#include "PlayerHud.generated.h"

/**
 * 
 */
UCLASS()
class UE5_COMBATPROTO_API APlayerHud : public AHUD
{
	GENERATED_BODY()
public:
	APlayerHud();

	// Reference to the player
	AMainCharacter* MainCharacter;
	
};
