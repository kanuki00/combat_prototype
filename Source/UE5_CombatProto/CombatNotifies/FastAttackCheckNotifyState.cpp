// Fill out your copyright notice in the Description page of Project Settings.


#include "FastAttackCheckNotifyState.h"
#include "../Characters/V1/MainCharacter.h"


void UFastAttackCheckNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	// For debugging
	if (GEngine && false)
	{
		GEngine->AddOnScreenDebugMessage(1, 1.0f, FColor::Yellow, __FUNCTION__);
	}
}

void UFastAttackCheckNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	if (Cast<APlayerCharacter>(MeshComp->GetOwner())) {
		APlayerCharacter* Player = Cast<APlayerCharacter>(MeshComp->GetOwner());
		//Player->CheckFastAttackPressed();
		Player->CheckAttackPressed();
	}
}

void UFastAttackCheckNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (Cast<APlayerCharacter>(MeshComp->GetOwner())) {
		APlayerCharacter* Player = Cast<APlayerCharacter>(MeshComp->GetOwner());
		//Player->ContinueFastAttack();
		Player->ContinueAttack();
	}
}
