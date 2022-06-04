// Fill out your copyright notice in the Description page of Project Settings.


#include "GeneralNotifies.h"
#include "Characters/BaseCharacter.h"
#include "Characters/EnemyCharacter.h"

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
	Char = Cast<ABaseCharacter>(MeshComp->GetOwner());
	if (Char)
	{
		Char->EndRoll();
	}
}

void UResetEnemyAtkCooldownNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	EChar = Cast<AEnemyCharacter>(MeshComp->GetOwner());
	if (EChar)
	{
		EChar->AttackCoolingDown = false;
		EChar->AttackEndCooldown = true;
	}
}
