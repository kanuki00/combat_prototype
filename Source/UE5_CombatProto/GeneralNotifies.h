// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GeneralNotifies.generated.h"


class ABaseCharacter;

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
