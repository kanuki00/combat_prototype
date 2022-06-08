// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlayerCharacterV2.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"

ABasePlayerCharacterV2::ABasePlayerCharacterV2()
{
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Boom"));
	CameraBoom->SetupAttachment(RootComponent);
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBoom, FName("Socket"));

	bUseControllerRotationYaw = false;

	CameraBoom->bUsePawnControlRotation = true;

	// Moving mesh to a better initial location
	FVector MeshLoc = FVector(0.0f, 0.0f, -(GetCapsuleComponent()->GetScaledCapsuleHalfHeight()));
	FRotator MeshRot = FRotator(0.0f, -90.0f, 0.0f);
	GetMesh()->SetRelativeLocationAndRotation(MeshLoc, MeshRot);
}

void ABasePlayerCharacterV2::BeginPlay()
{
	Super::BeginPlay();
}

void ABasePlayerCharacterV2::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	OrientToMovementInput(DeltaTime);

	if (true)
	{
		WalkMovementFacing();
	}
	else {
		WalkMovementController();
	}

	CameraMovement();
}

void ABasePlayerCharacterV2::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	PlayerInputComponent->BindAxis("Move_Forward_Backward", this, & ABasePlayerCharacterV2::MoveForwardBind);
	PlayerInputComponent->BindAxis("Move_Right_Left", this, & ABasePlayerCharacterV2::MoveRightBind);
	PlayerInputComponent->BindAxis("Look_Up_Down", this, & ABasePlayerCharacterV2::LookUpBind);
	PlayerInputComponent->BindAxis("Look_Right_Left", this, & ABasePlayerCharacterV2::LookRightBind);
}

void ABasePlayerCharacterV2::MoveForwardBind(float Axis)
{
	MovementInput = FVector(Axis, MovementInput.Y, 0.0f);
}

void ABasePlayerCharacterV2::MoveRightBind(float Axis)
{
	MovementInput = FVector(MovementInput.X, Axis, 0.0f);
}

void ABasePlayerCharacterV2::LookUpBind(float Axis)
{
	CameraInput = FVector(Axis, CameraInput.Y, 0.0f);
}

void ABasePlayerCharacterV2::LookRightBind(float Axis)
{
	CameraInput = FVector(CameraInput.X, Axis, 0.0f);
}

void ABasePlayerCharacterV2::OrientToMovementInput(float DeltaTime, float RotationSpeed)
{
	if (MovementInput.Length() > 0.0f)
	{
		FVector LookDirection = FRotator(0.0f, GetControlRotation().Yaw, 0.0f).RotateVector(MovementInput);
		CharacterFacing = UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation(), this->GetActorLocation() + LookDirection);
	}

	this->SetActorRotation(UKismetMathLibrary::RInterpTo_Constant(this->GetActorRotation(), CharacterFacing, DeltaTime, RotationSpeed));
}

void ABasePlayerCharacterV2::WalkMovementController()
{
	float Scale = UKismetMathLibrary::ClampVectorSize(MovementInput, 0.0f, 1.0f).Length();
	FRotator CtrlRotYaw = FRotator(0.0f, GetControlRotation().Yaw, 0.0f);
	FVector MovementInputCopy = MovementInput.GetSafeNormal();
	AddMovementInput(CtrlRotYaw.RotateVector(MovementInputCopy), Scale);
}

void ABasePlayerCharacterV2::WalkMovementFacing()
{
	float Scale = UKismetMathLibrary::ClampVectorSize(MovementInput, 0.0f, 1.0f).Length();
	AddMovementInput(this->GetActorForwardVector(), Scale);
}

void ABasePlayerCharacterV2::CameraMovement()
{
	AddControllerPitchInput(CameraInput.X);
	AddControllerYawInput(CameraInput.Y);
}
