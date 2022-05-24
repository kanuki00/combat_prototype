// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackSetterNotifies.h"
#include "../Characters/BaseCharacter.h"

void UEnableCharacterRotation::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
}

void UDisableCharacterRotation::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{

}

void UEnableDamageDealing::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	ABaseCharacter* Character;
	if (Cast<ABaseCharacter>(MeshComp->GetOwner()))
	{
		Character = Cast<ABaseCharacter>(MeshComp->GetOwner());
		Character->SetApplyDamageEnabled(true);
	}
}

void UDisableDamageDealing::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	ABaseCharacter* Character;
	if (Cast<ABaseCharacter>(MeshComp->GetOwner()))
	{
		Character = Cast<ABaseCharacter>(MeshComp->GetOwner());
		Character->SetApplyDamageEnabled(false);
	}
}
