// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CombatInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

class UE5_COMBATPROTO_API ICombatInterface
{
	GENERATED_BODY()

public:

	virtual void WeaponHit(AActor* HitActor);
};