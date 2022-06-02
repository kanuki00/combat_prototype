//This is unused

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "StrongAttackEndedNotify.generated.h"

/**
 * 
 */
UCLASS(Deprecated)
class UE5_COMBATPROTO_API UDEPRECATED_StrongAttackEndedNotify : public UAnimNotify
{
	GENERATED_BODY()
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	
};
