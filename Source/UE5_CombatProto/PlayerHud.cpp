// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHud.h"
#include "CombatInterface.h"


APlayerHud::APlayerHud()
{
	if (false)//(PlayerOwner->GetClass()->ImplementsInterface(UCombatInterface::StaticClass()))
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Yellow, TEXT("Hud: owner implements!"));
	}
}
