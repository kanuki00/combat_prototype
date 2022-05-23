// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CombatInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

class ICombatInterface
{
	GENERATED_BODY()
public:
	// Blueprint only interface functions
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		int BP_GetMaxHealth();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		int BP_GetHealth();

	// C++ interface functions
	virtual int GetMaxHealth();
	virtual int GetHealth();
};