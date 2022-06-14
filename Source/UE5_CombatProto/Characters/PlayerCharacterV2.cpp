// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacterV2.h"
#include "../GameComponents/TargetingComponent.h"

APlayerCharacterV2::APlayerCharacterV2()
{
	PrimaryActorTick.bCanEverTick = true;

	TargetingComponent = CreateDefaultSubobject<UTargetingComponent>(TEXT("Targeting"));
}

void APlayerCharacterV2::BeginPlay()
{
	Super::BeginPlay();
}

void APlayerCharacterV2::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APlayerCharacterV2::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Target", IE_Pressed, this, &APlayerCharacterV2::TargetPressedBind);
	PlayerInputComponent->BindAction("Target", IE_Released, this, &APlayerCharacterV2::TargetReleasedBind);
}

void APlayerCharacterV2::TargetPressedBind()
{
	TargetingComponent->GetNewTarget();
}

void APlayerCharacterV2::TargetReleasedBind()
{
	TargetingComponent->ClearTarget();
}
