// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacterV2.h"
#include "Components/CapsuleComponent.h"

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
}

void AEnemyCharacterV2::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
