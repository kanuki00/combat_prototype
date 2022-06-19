// This is unused


#include "StrongAttackCheckNotifyState.h"
#include "../Characters/V1/MainCharacter.h"

void UDEPRECATED_StrongAttackCheckNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
}

void UDEPRECATED_StrongAttackCheckNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	if (Cast<APlayerCharacter>(MeshComp->GetOwner())) {
		APlayerCharacter* Player = Cast<APlayerCharacter>(MeshComp->GetOwner());
		//Player->CheckStrongAttackPressed();
		Player->CheckAttackPressed();
	}
}

void UDEPRECATED_StrongAttackCheckNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (Cast<APlayerCharacter>(MeshComp->GetOwner())) {
		APlayerCharacter* Player = Cast<APlayerCharacter>(MeshComp->GetOwner());
		//Player->ContinueStrongAttack();
		Player->ContinueAttack();
	}
}
