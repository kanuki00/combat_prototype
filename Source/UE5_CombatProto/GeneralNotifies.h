// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GeneralNotifies.generated.h"


class ABaseCharacter;
class AEnemyCharacter;

UCLASS()
class UE5_COMBATPROTO_API UDeathNotify : public UAnimNotify
{
	GENERATED_BODY()
public:
	ABaseCharacter* Char;
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};

UCLASS()
class UE5_COMBATPROTO_API URollEndNotify : public UAnimNotify
{
	GENERATED_BODY()
public:
	ABaseCharacter* Char;
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};

// Notify used by enemies to reset attack cooldown
UCLASS()
class UE5_COMBATPROTO_API UResetEnemyAtkCooldownNotify : public UAnimNotify
{
	GENERATED_BODY()
	AEnemyCharacter* EChar;
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};