// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../CombatInterface.h"
#include "UObject/Interface.h"
#include "BaseCharacter.generated.h"

// Interface for animations
UINTERFACE(MinimalAPI, Blueprintable)
class UCharacterAnimationInterface : public UInterface
{
	GENERATED_BODY()
};

class UE5_COMBATPROTO_API ICharacterAnimationInterface
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Animation Interface")
		float GetMovementInputStrength();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Animation Interface")
		FVector GetMovementInputDirection();
};

UCLASS()
class UE5_COMBATPROTO_API ABaseCharacter : public ACharacter, public ICombatInterface, public ICharacterAnimationInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		int MaxHealth = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		int Health = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		bool CanApplyDamage = false;

	void Cooldown(bool & CoolingDown, float & CooldownTimer, float & CoolDownLength, float DeltaTime);
	void WasPressed(bool & WasPressed, bool & Pressed, bool & PressedCache);

	// Death Montage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Asset References|Animations")
		UAnimMontage* DeathAnimation = nullptr;
	// Roll Montage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Asset References|Animations")
		UAnimMontage* RollAnimation = nullptr;
	// Attack Montages
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Asset References|Animations")
		UAnimMontage* FastAttack = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Asset References|Animations")
		UAnimMontage* StrongAttack = nullptr;


	UFUNCTION(BlueprintCallable)
		void Death();

	virtual void UniqueDeath();

	

public:	
	void RagdollChar();

	bool IsDead = false;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(BlueprintReadOnly)
		bool IsSprinting = false; // Used for animation.

	// Functions for getting health of character.
	UFUNCTION()
		int GetMaxHealth();

	UFUNCTION()
		int GetHealth();

	// For setting damage dealing to enabled or disabled.
	UFUNCTION()
		void SetApplyDamageEnabled(bool NewDamage = true);

	UFUNCTION()
		virtual void SetCharacterRotationEnabled(bool NewRotate = true);

};
