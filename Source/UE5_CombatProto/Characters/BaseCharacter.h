// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "../CombatInterface.h"
#include "BaseCharacter.generated.h"

/*

*/
UCLASS()
class UE5_COMBATPROTO_API ABaseCharacter : public ACharacter, public ICombatInterface
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
	// Attack Montages
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Asset References|Animations")
		UAnimMontage* FastAttack = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Asset References|Animations")
		UAnimMontage* StrongAttack = nullptr;


	UFUNCTION(BlueprintCallable)
		void Death();

	bool IsDead = false;

public:	
	void RagdollChar();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

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
