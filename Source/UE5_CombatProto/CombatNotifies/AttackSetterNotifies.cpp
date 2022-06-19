// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackSetterNotifies.h"
#include "../Characters/V1/BaseCharacter.h"

void UEnableCharacterRotation::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	ABaseCharacter* Character;
	if (Cast<ABaseCharacter>(MeshComp->GetOwner()))
	{
		Character = Cast<ABaseCharacter>(MeshComp->GetOwner());
		Character->SetCharacterRotationEnabled(true);
	}
}

void UDisableCharacterRotation::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	ABaseCharacter* Character;
	if (Cast<ABaseCharacter>(MeshComp->GetOwner()))
	{
		Character = Cast<ABaseCharacter>(MeshComp->GetOwner());
		Character->SetCharacterRotationEnabled(false);
	}
}

void UEnableDamageDealing::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	// Enabling character's damage dealing
	ABaseCharacter* Character;
	if (Cast<ABaseCharacter>(MeshComp->GetOwner()))
	{
		Character = Cast<ABaseCharacter>(MeshComp->GetOwner());
		Character->SetApplyDamageEnabled(true);
	}
}

void UDisableDamageDealing::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	// Disabling character's damage dealing
	ABaseCharacter* Character;
	if (Cast<ABaseCharacter>(MeshComp->GetOwner()))
	{
		Character = Cast<ABaseCharacter>(MeshComp->GetOwner());
		Character->SetApplyDamageEnabled(false);
	}
}
