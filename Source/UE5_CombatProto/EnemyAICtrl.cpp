// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAICtrl.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

AEnemyAICtrl::AEnemyAICtrl()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AEnemyAICtrl::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	bool SeesPlayer = false;
	if (SensedActors.Num() > 0) SeesPlayer = Cast<ACharacter>(SensedActors[0]) == UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	GetBlackboardComponent()->SetValueAsBool(FName("SeesPlayer"), SeesPlayer);
}
