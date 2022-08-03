// Fill out your copyright notice in the Description page of Project Settings.
//This needs to be moved into same files as the notify states!!!

#include "FastAttackEndedNotify.h"
#include "../Characters/V1/MainCharacter.h"
#include "../Characters/PlayerCharacterV2.h"

void UFastAttackEndedNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (Cast<APlayerCharacter>(MeshComp->GetOwner()))
	{
		APlayerCharacter* MainCharacter = Cast<APlayerCharacter>(MeshComp->GetOwner());
		MainCharacter->EndAttack();
	}

	// V2
	APlayerCharacterV2* PlayerV2 = Cast<APlayerCharacterV2>(MeshComp->GetOwner());
	if (PlayerV2)
	{
		PlayerV2->EndAttack();
	}
}
