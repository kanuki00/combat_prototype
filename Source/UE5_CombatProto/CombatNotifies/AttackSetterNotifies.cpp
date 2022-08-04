// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackSetterNotifies.h"
#include "../Characters/V1/BaseCharacter.h"
#include "../Characters/BasePlayerCharacterV2.h"

void UEnableCharacterRotation::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	ABaseCharacter* Character;
	if (Cast<ABaseCharacter>(MeshComp->GetOwner()))
	{
		Character = Cast<ABaseCharacter>(MeshComp->GetOwner());
		Character->SetCharacterRotationEnabled(true);
	}

	// V2
	ABasePlayerCharacterV2* PlayerCharacterV2 = Cast<ABasePlayerCharacterV2>(MeshComp->GetOwner());
	if (PlayerCharacterV2)
	{
		PlayerCharacterV2->CanOrientToStickInput = true;
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

	// V2
	ABasePlayerCharacterV2* PlayerCharacterV2 = Cast<ABasePlayerCharacterV2>(MeshComp->GetOwner());
	if (PlayerCharacterV2)
	{
		PlayerCharacterV2->CanOrientToStickInput = false;
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

	// V2
	ABaseCharacterV2* CharacterV2 = Cast<ABaseCharacterV2>(MeshComp->GetOwner());
	if (CharacterV2)
	{
		CharacterV2->CanApplyDamage = true;
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

	// V2
	ABaseCharacterV2* CharacterV2 = Cast<ABaseCharacterV2>(MeshComp->GetOwner());
	if (CharacterV2)
	{
		CharacterV2->CanApplyDamage = false;
	}
}
