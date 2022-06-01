// Fill out your copyright notice in the Description page of Project Settings.


#include "GeneralNotifies.h"

void UDeathNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (Cast<ABaseCharacter>(MeshComp->GetOwner())) 
	{
		Char = Cast<ABaseCharacter>(MeshComp->GetOwner());
		Char->RagdollChar();
	}
}