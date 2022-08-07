// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacterV2.h"
#include "BasePlayerCharacterV2.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UBaseWidget;

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

	virtual void Input1PressedBind();
	virtual void Input1ReleasedBind();
	bool Input1Pressed = false;

	virtual void Input2PressedBind();
	virtual void Input2ReleasedBind();
	bool Input2Pressed = false;

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
	bool CanOrientToStickInput = true;
	float OrientSpeed = 480.0f;
	const float DefaultOrientSpeed = 480.0f;
protected:
	bool UseWalkMovementController = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HUD")
		UBaseWidget* HudWidget = nullptr;
private:

	FRotator CharacterFacing;
	void OrientToStickInput(float DeltaTime, float RotationSpeed = 720.0f);

	void WalkMovementController();
	void WalkMovementFacing();

	void CameraMovement();
};
