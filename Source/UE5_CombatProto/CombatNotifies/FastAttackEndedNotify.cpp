// Fill out your copyright notice in the Description page of Project Settings.


#include "FastAttackEndedNotify.h"
#include "../Characters/MainCharacter.h"

void UFastAttackEndedNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (Cast<APlayerCharacter>(MeshComp->GetOwner()))
	{
		APlayerCharacter* MainCharacter = Cast<APlayerCharacter>(MeshComp->GetOwner());
		MainCharacter->EndAttack();
	}
}
