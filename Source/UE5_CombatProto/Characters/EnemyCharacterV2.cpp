// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacterV2.h"
#include "Components/CapsuleComponent.h"
#include "../Controllers/EnemyAICV2.h"

AEnemyCharacterV2::AEnemyCharacterV2()
{
	PrimaryActorTick.bCanEverTick = true;
	
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);
	
}

void AEnemyCharacterV2::BeginPlay()
{
	Super::BeginPlay();

	AEnemyAICV2* Ctrl;
	Ctrl = Cast<AEnemyAICV2>(GetController());
	if (Ctrl)
	{
		Ctrl->SetGenericTeamId(FGenericTeamId(Team));
		Ctrl->Shout();
	}
}

void AEnemyCharacterV2::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEnemyCharacterV2::UniqueTakeDamage()
{
	DEBUG_MSG(-1, 4, Red, TEXT("Took Unique Damage"));
}

void AEnemyCharacterV2::UniqueDeath()
{
	this->Destroy();
}
