// Fill out your copyright notice in the Description page of Project Settings.


#include "StrongAttackCheckNotifyState.h"
#include "../Characters/MainCharacter.h"

void UStrongAttackCheckNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
}

void UStrongAttackCheckNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	if (Cast<AMainCharacter>(MeshComp->GetOwner())) {
		AMainCharacter* Player = Cast<AMainCharacter>(MeshComp->GetOwner());
		Player->CheckStrongAttackPressed();
	}
}

void UStrongAttackCheckNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (Cast<AMainCharacter>(MeshComp->GetOwner())) {
		AMainCharacter* Player = Cast<AMainCharacter>(MeshComp->GetOwner());
		Player->ContinueStrongAttack();
	}
}
