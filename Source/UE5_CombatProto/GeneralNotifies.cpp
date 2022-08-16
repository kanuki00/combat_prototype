// Fill out your copyright notice in the Description page of Project Settings.


#include "GeneralNotifies.h"
#include "Characters/V1/BaseCharacter.h"
#include "Characters/V1/EnemyCharacter.h"
#include "Characters/PlayerCharacterV2.h"
#include "Characters/EnemyCharacterV2.h"

void UDeathNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (Cast<ABaseCharacter>(MeshComp->GetOwner())) 
	{
		Char = Cast<ABaseCharacter>(MeshComp->GetOwner());
		Char->RagdollChar();
	}
}

void URollEndNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	// V1
	Char = Cast<ABaseCharacter>(MeshComp->GetOwner());
	if (Char)
	{
		Char->EndRoll();
	}
	// V2
	APlayerCharacterV2* Player = Cast<APlayerCharacterV2>(MeshComp->GetOwner());
	if (!Player) return;
	Player->EndRoll();
}

void UResetEnemyAtkCooldownNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	EChar = Cast<AEnemyCharacter>(MeshComp->GetOwner());
	if (EChar)
	{
		EChar->AttackCoolingDown = false;
		EChar->AttackEndCooldown = true;
	}

	// V2
	
	AEnemyCharacterV2* EnemyV2 = Cast<AEnemyCharacterV2>(MeshComp->GetOwner());
	if (EnemyV2)
	{
		EnemyV2->CanStartAttack = true;
	}
}

void UHitEndNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	AEnemyCharacterV2* Char = Cast<AEnemyCharacterV2>(MeshComp->GetOwner());
	if (Char == nullptr) return;
	Char->CharMovComp->SetMovementMode(EMovementMode::MOVE_Walking);
	Char->CanStartAttack = true;
}
