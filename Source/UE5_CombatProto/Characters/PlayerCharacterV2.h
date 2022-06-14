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

protected:
	UPROPERTY(EditAnywhere)
		UTargetingComponent* TargetingComponent;
};
