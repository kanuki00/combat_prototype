// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacterV2.h"
#include "BasePlayerCharacterV2.generated.h"

class USpringArmComponent;
class UCameraComponent;

UCLASS()
class UE5_COMBATPROTO_API ABasePlayerCharacterV2 : public ABaseCharacterV2
{
	GENERATED_BODY()
public:
	// *********************************
	// Constructor
	// *********************************
	ABasePlayerCharacterV2();

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

	void MoveForwardBind(float Axis);
	void MoveRightBind(float Axis);
	void LookUpBind(float Axis);
	void LookRightBind(float Axis);

	void Input1PressedBind();
	void Input1ReleasedBind();
	bool Input1Pressed = false;

	FVector CameraInput;

protected:
	// *********************************
	// Components
	// *********************************
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
		USpringArmComponent* CameraBoom;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
		UCameraComponent* Camera;

	// *********************************
	// Character functionality 
	// *********************************
public:
protected:
	bool CanOrientToMovementInput = true;

	bool UseWalkMovementController = false;
private:

	FRotator CharacterFacing;
	void OrientToMovementInput(float DeltaTime, float RotationSpeed = 720.0f);

	void WalkMovementController();
	void WalkMovementFacing();

	void CameraMovement();
};
