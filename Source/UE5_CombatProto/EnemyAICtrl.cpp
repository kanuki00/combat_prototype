// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAICtrl.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/EnemyCharacter.h"

AEnemyAICtrl::AEnemyAICtrl()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AEnemyAICtrl::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (SensedActors.Num() > 0) { SeesPlayer = Cast<ACharacter>(SensedActors[0]) == UGameplayStatics::GetPlayerCharacter(GetWorld(), 0); }
	else { SeesPlayer = false; }

	GetBlackboardComponent()->SetValueAsBool(FName("SeesPlayer"), SeesPlayer);

	float DistToPlayer = FVector(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->GetActorLocation() - GetPawn()->GetActorLocation()).Length();
	if (DistToPlayer <= AttackRange)
	{
		InRangeOfPlayer = true;
	}
	else {
		InRangeOfPlayer = false;
	}

	if (InRangeOfPlayer && SeesPlayer)
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(1, 1, FColor::Red, TEXT("Trying to attack"));
		Cast<AEnemyCharacter>(GetPawn())->Attack();
	}
}
