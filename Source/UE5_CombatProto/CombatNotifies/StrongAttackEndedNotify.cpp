// Fill out your copyright notice in the Description page of Project Settings.


#include "StrongAttackEndedNotify.h"
#include "../Characters/MainCharacter.h"

void UStrongAttackEndedNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (Cast<AMainCharacter>(MeshComp->GetOwner()))
	{
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(MeshComp->GetOwner());
		MainCharacter->EndAttack();
	}
}
