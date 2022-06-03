// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAICtrl.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/EnemyCharacter.h"
#include "GameFramework/Character.h"

AEnemyAICtrl::AEnemyAICtrl()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AEnemyAICtrl::BeginPlay()
{
	Super::BeginPlay();

	Player0 = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	//if (GEngine) GEngine->AddOnScreenDebugMessage(10, 1, FColor::Red, Player0->GetName());
}

void AEnemyAICtrl::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, TEXT("Ticking"));
	
	if (SensedActors.Num() > 0) { SeesPlayer = Cast<ACharacter>(SensedActors[0]) == Player0; }
	else { SeesPlayer = false; }

	GetBlackboardComponent()->SetValueAsBool(FName("SeesPlayer"), SeesPlayer);

	if (Player0 && Player0->IsPendingKill() == false) // if we don't check that the player is valid, game will crash upon destruction of player character object.
	{
		DistToPlayer = FVector(Player0->GetActorLocation() - GetPawn()->GetActorLocation()).Length();
	}
	else 
	{ 
		DistToPlayer = 100000000.0f; // Just some massive number if player doesn't exist.
	}
	
	if (DistToPlayer <= AttackRange)
	{
		InRangeOfPlayer = true;
	}
	else {
		InRangeOfPlayer = false;
	}

	if (InRangeOfPlayer && SeesPlayer)
	{
		//if (GEngine) GEngine->AddOnScreenDebugMessage(1, 1, FColor::Red, TEXT("Trying to attack"));
		Cast<AEnemyCharacter>(GetPawn())->Attack();
	}
}
