// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacterV2.h"
#include "Components/CapsuleComponent.h"

#include "BrainComponent.h"
//#include "Engine/EngineTypes.h"
#include "Kismet/KismetMathLibrary.h"
#include "../GameMacros.h"

AEnemyCharacterV2::AEnemyCharacterV2()
{
	PrimaryActorTick.bCanEverTick = true;

	CharMovComp->MaxWalkSpeed = 300.0f;

	bUseControllerRotationYaw = false;
	CharMovComp->bUseControllerDesiredRotation = true;
	CharMovComp->bOrientRotationToMovement = false;

	StrafeEnabled = true;
}

void AEnemyCharacterV2::BeginPlay()
{
	Super::BeginPlay();

	AIController = Cast<AEnemyAICV2>(GetController());

	if (AIController)
	{
		AIController->SetGenericTeamId(FGenericTeamId(Team));
		//Ctrl->Shout();
	}
}

void AEnemyCharacterV2::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// Enemy's movement input is current velocity. 
	MovementInput = GetVelocity().GetSafeNormal();
	MovementInput *= CharMovComp->MaxWalkSpeed / DefaultMaxWalkSpeed;

	FlipBoolAfterDelay(CanRandomLeftRight, false, RandomLeftRightCooldownTimer, RandomLeftRightCooldown, DeltaTime);

	/*
	if (Left)
	{
		DEBUG_MESSAGE(-1, DeltaTime, FColor::Red, TEXT("Left"))
	}
	else
	{
		DEBUG_MESSAGE(-1, DeltaTime, FColor::Blue, TEXT("Right"))
	}
	//*/ // Testing AI's left right choosing.
}

void AEnemyCharacterV2::RandomLeftRight()
{
	if (!CanRandomLeftRight) return;
	CanRandomLeftRight = false;
	RandomLeftRightCooldown = UKismetMathLibrary::RandomFloatInRange(15.0f, 25.0f);
	Left = UKismetMathLibrary::RandomBool();
}

void AEnemyCharacterV2::RotateToActor(AActor* Actor, float DeltaTime, float Speed)
{
	FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation(), Actor->GetActorLocation());
	Rotation = FRotator(0.0f, Rotation.Yaw, 0.0f); 

	SetActorRotation(UKismetMathLibrary::RInterpTo_Constant(GetActorRotation(), Rotation, DeltaTime, Speed));
}

void AEnemyCharacterV2::StartAttack()
{
	if (AttackAnimations.Num() == 0 || !CanStartAttack) return;
	CanStartAttack = false;
	int AttackNum = UKismetMathLibrary::RandomIntegerInRange(0, AttackAnimations.Num() - 1);
	PlayAnimMontage(AttackAnimations[AttackNum]);
}

void AEnemyCharacterV2::UniqueTakeDamage(AActor* DamageCauser)
{
	// Focus the controller on damage causer when receiving damage.
	AAIController* AIC = Cast<AAIController>(GetController());
	AIC->SetFocus(DamageCauser, EAIFocusPriority::Gameplay);
	// Disabling movement when damage is taken. Will be re-enabled soon by hit animation.
	//CharMovComp->DisableMovement();
	//CharMovComp->SetMovementMode(EMovementMode::MOVE_Walking);
}

void AEnemyCharacterV2::UniqueDeath()
{
	TargetWidget->RemoveFromParent();
	SuggestWidget->RemoveFromParent();
	// Stopping AI
	if (AIController)
	{
		AIController->GetBrainComponent()->StopLogic(FString("Death"));
	}
	GetCapsuleComponent()->SetCollisionProfileName("NoCollision");
	// Ragdolling
	GetMesh()->SetCollisionProfileName("Ragdoll");
	GetMesh()->bPauseAnims = true;
	GetMesh()->SetSimulatePhysics(true);

}
