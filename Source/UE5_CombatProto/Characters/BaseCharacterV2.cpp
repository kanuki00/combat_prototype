// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacterV2.h"
#include "CollisionShape.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/CapsuleComponent.h"

// *********************************
// Constructor
// *********************************
ABaseCharacterV2::ABaseCharacterV2()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Moving mesh to a better initial location
	FVector MeshLoc = FVector(0.0f, 0.0f, -(GetCapsuleComponent()->GetScaledCapsuleHalfHeight()));
	FRotator MeshRot = FRotator(0.0f, -90.0f, 0.0f);
	GetMesh()->SetRelativeLocationAndRotation(MeshLoc, MeshRot);
}

// *********************************
// Begin Play
// *********************************
void ABaseCharacterV2::BeginPlay()
{
	Super::BeginPlay();
}

// *********************************
// Tick
// *********************************
void ABaseCharacterV2::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FlipBoolAfterDelay(TakeDamageCoolingDown, true, TakeDamageCooldownTimer, TakeDamageCooldown, DeltaTime);
	FlipBoolAfterDelay(SimpleAttackCoolingDown, true, S_A_CooldownTimer, S_A_Cooldown, DeltaTime);
}

// *********************************
// Bind
// *********************************
void ABaseCharacterV2::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

// *********************************
// Character functionality 
// *********************************

float ABaseCharacterV2::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	/* Debug */if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, TEXT("Took Damage"));
	if (CanTakeDamage && !TakeDamageCoolingDown)
	{
		TakeDamageCoolingDown = true;
		Health -= DamageAmount; 
		UniqueTakeDamage();
		if (Health <= 0.0f)
		{
			Death();
		}
	}

	return DamageAmount;
}

void ABaseCharacterV2::UniqueTakeDamage()
{
}

void ABaseCharacterV2::FlipBoolAfterDelay(bool& Bool, bool ValueToFlipFrom, float& Timer, float& Delay, float DeltaTime)
{
	if (Bool == ValueToFlipFrom)
	{
		Timer += DeltaTime;
		if (Timer >= Delay)
		{
			Bool = !ValueToFlipFrom;
			Timer = 0.0f;
		}
	}
}

void ABaseCharacterV2::Death()
{
	IsDead = true;
	Health = 0.0f;
	UniqueDeath();
}

void ABaseCharacterV2::UniqueDeath()
{
}

void ABaseCharacterV2::SimpleAttack(float Damage)
{
	if (!SimpleAttackCoolingDown)
	{
		SimpleAttackCoolingDown = true;
		FVector TraceLoc = this->GetActorLocation() + this->GetActorForwardVector() * 70.0f + this->GetActorUpVector() * 30.0f;
		TArray<AActor*> Ignore;
		TArray<struct FHitResult> HitResults;

		UKismetSystemLibrary::CapsuleTraceMulti(GetWorld(), TraceLoc, TraceLoc, 30.0f, 50.0f, ETraceTypeQuery::TraceTypeQuery1, true, Ignore, EDrawDebugTrace::ForOneFrame, HitResults, false);

		FDamageEvent DmgEvent;
		for (int i = 0; i < HitResults.Num(); i++)
		{
			/* Debug */if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, TEXT("Hit Something"));
			HitResults[i].GetActor()->TakeDamage(Damage, DmgEvent, GetController(), this);
		}
	}
}
