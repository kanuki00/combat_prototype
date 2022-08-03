// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasePlayerCharacterV2.h"
#include "PlayerCharacterV2.generated.h"

class UTargetingComponent;

UCLASS()
class UE5_COMBATPROTO_API APlayerCharacterV2 : public ABasePlayerCharacterV2
{
	GENERATED_BODY()
public:
	// *********************************
	// Constructor
	// *********************************
	APlayerCharacterV2();

protected:
	// *********************************
	// Begin Play
	// *********************************
	virtual void BeginPlay() override;

public:
	// *********************************
	// Tick
	// *********************************
	virtual void Tick(float DeltaTime) override;

	// *********************************
	// Bind
	// *********************************
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void TargetPressedBind();
	void TargetReleasedBind();

	void Input1PressedBind() override;
	void Input1ReleasedBind() override;

	void Input2PressedBind() override;
	void Input2ReleasedBind() override;

	bool Input1PressedCache = false;
	bool Input2PressedCache = false;

	bool Input1WasPressed = false;
	bool Input2WasPressed = false;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UTargetingComponent* TargetingComponent;

	void StartFastAttack();

	void StartStrongAttack();

	void CheckAttackPressed();

	void ContinueAttack();

	void TransToStrongAttack();

	void TransToFastAttack();

	void EndAttack();

	bool CanStartFastAttack = true;
	bool CanStartStrongAttack = true;

	bool ShouldContinueFastAttack = false;
	bool ShouldContinueStrongAttack = false;

	bool FastAttackCoolingDown = false;
	bool StrongAttackCoolingDown = false;

	float FastAttackCoolDownTimer = 0.0f;
	float StrongAttackCoolDownTimer = 0.0f;

	FName CurrentStrongAttackSection;
	FName CurrentStrongAttackSectionCache;

	FName CurrentFastAttackSection;
	FName CurrentFastAttackSectionCache;

protected:
	virtual void UniqueDeath() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		UAnimMontage* FastAttackAnimation = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		UAnimMontage* StrongAttackAnimation = nullptr;
};
