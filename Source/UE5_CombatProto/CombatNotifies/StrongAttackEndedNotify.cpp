// This is unused


#include "StrongAttackEndedNotify.h"
#include "../Characters/MainCharacter.h"

void UDEPRECATED_StrongAttackEndedNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (Cast<APlayerCharacter>(MeshComp->GetOwner()))
	{
		APlayerCharacter* MainCharacter = Cast<APlayerCharacter>(MeshComp->GetOwner());
		MainCharacter->EndAttack();
	}
}
