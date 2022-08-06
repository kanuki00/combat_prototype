// 


#include "AttackAnimNotifies.h"
#include "../Characters/V1/MainCharacter.h"
#include "../Characters/PlayerCharacterV2.h"


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
	// V1 Deprecated
	if (Cast<APlayerCharacter>(MeshComp->GetOwner())) {
		APlayerCharacter* Player = Cast<APlayerCharacter>(MeshComp->GetOwner());
		//Player->CheckFastAttackPressed();
		Player->CheckAttackPressed();
	}

	// V2
	APlayerCharacterV2* PlayerV2 = Cast<APlayerCharacterV2>(MeshComp->GetOwner());
	if (PlayerV2)
	{
		PlayerV2->CheckAttackPressed();
	}
}

void UFastAttackCheckNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	// V1 Deprecated
	if (Cast<APlayerCharacter>(MeshComp->GetOwner())) {
		APlayerCharacter* Player = Cast<APlayerCharacter>(MeshComp->GetOwner());
		//Player->ContinueFastAttack();
		Player->ContinueAttack();
	}

	// V2
	APlayerCharacterV2* PlayerV2 = Cast<APlayerCharacterV2>(MeshComp->GetOwner());
	if (PlayerV2)
	{
		PlayerV2->ContinueAttack();
	}
}

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