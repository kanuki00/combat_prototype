// Fill out your copyright notice in the Description page of Project Settings.


#include "FastAttackEndedNotify.h"
#include "MainCharacter.h"

void UFastAttackEndedNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (Cast<AMainCharacter>(MeshComp->GetOwner()))
	{
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(MeshComp->GetOwner());
		MainCharacter->EndAttack();
	}
}
