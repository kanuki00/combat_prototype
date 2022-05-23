// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHud.h"


APlayerHud::APlayerHud()
{
	MainCharacter = Cast<AMainCharacter>(PlayerOwner);

	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Yellow, TEXT("O"));
}
