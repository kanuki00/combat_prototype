// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacterV2.h"
#include "Components/CapsuleComponent.h"
#include "../Controllers/EnemyAICV2.h"
//#include "Engine/EngineTypes.h"
#include "Kismet/KismetMathLibrary.h"

AEnemyCharacterV2::AEnemyCharacterV2()
{
	PrimaryActorTick.bCanEverTick = true;

	CharMovComp->MaxWalkSpeed = 300.0f;

	bUseControllerRotationYaw = false;
	CharMovComp->bUseControllerDesiredRotation = true;
	CharMovComp->bOrientRotationToMovement = false;
}

void AEnemyCharacterV2::BeginPlay()
{
	Super::BeginPlay();

	AEnemyAICV2* Ctrl;
	Ctrl = Cast<AEnemyAICV2>(GetController());
	if (Ctrl)
	{
		Ctrl->SetGenericTeamId(FGenericTeamId(Team));
		//Ctrl->Shout();
	}
}

void AEnemyCharacterV2::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEnemyCharacterV2::RotateToActor(AActor* Actor, float DeltaTime, float Speed)
{
	FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation(), Actor->GetActorLocation());
	Rotation = FRotator(0.0f, Rotation.Yaw, 0.0f); 

	SetActorRotation(UKismetMathLibrary::RInterpTo_Constant(GetActorRotation(), Rotation, DeltaTime, Speed));
}

void AEnemyCharacterV2::UniqueTakeDamage(AActor* DamageCauser)
{
	// Focus the controller on damage causer when receiving damage.
	AAIController* AIC = Cast<AAIController>(GetController());
	AIC->SetFocus(DamageCauser, EAIFocusPriority::Gameplay);
}

void AEnemyCharacterV2::UniqueDeath()
{
	this->Destroy();
}
