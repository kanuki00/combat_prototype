// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"
#include "../AICtrlBase.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/UnrealMathUtility.h"

AEnemyCharacter::AEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	DamageStrength = 50;
}

float AEnemyCharacter::GetMovementInputStrength_Implementation()
{
	float result = this->GetVelocity().Length() / (600.0f*1.2f); // Max Walk speed * Mesh Scale.
	return result;
}

void AEnemyCharacter::Attack()
{
	if (!IsDead && !AttackCoolingDown && !AttackEndCooldown)
	{
		AttackCoolingDown = true;
		if (FastAttack) PlayAnimMontage(FastAttack);
	}
}

void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (CanRotateToPlayer)
	{
		RotateToPlayer(DeltaTime);
	}

	Cooldown(AttackEndCooldown, AttackEndCooldownTimer, AttackEndCooldownLength, DeltaTime);
}

void AEnemyCharacter::UniqueDeath()
{
	Cast<ABaseAICtrl>(GetController())->DisableAI();
}

void AEnemyCharacter::RotateToPlayer(float DeltaTime)
{
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation(), PlayerCharacter->GetActorLocation());
	Rotation = FRotator(0.0f, Rotation.Yaw, 0.0f); // Only rotate on plane
	this->SetActorRotation(FMath::RInterpConstantTo(this->GetActorRotation(), Rotation, DeltaTime, 720.0f));
}

void AEnemyCharacter::SetCharacterRotationEnabled(bool NewRotate)
{
	CanRotateToPlayer = NewRotate;
}
