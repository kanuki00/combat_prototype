// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacterV2.h"
#include "../GameComponents/TargetingComponent.h"
#include "Kismet/KismetMathLibrary.h"

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

	Input1WasPressed = false;
	if (Input1Pressed && !Input1PressedCache) Input1WasPressed = true;
	Input1PressedCache = Input1Pressed;

	Input2WasPressed = false;
	if (Input2Pressed && !Input2PressedCache) Input2WasPressed = true;
	Input2PressedCache = Input2Pressed;

	// most likely dont need two of these...
	CurrentFastAttackSection = GetMesh()->GetAnimInstance()->Montage_GetCurrentSection();
	CurrentStrongAttackSection = GetMesh()->GetAnimInstance()->Montage_GetCurrentSection();

	FlipBoolAfterDelay(StrongAttackCoolingDown, true, StrongAttackCoolDownTimer, 0.5f, DeltaTime);
	FlipBoolAfterDelay(FastAttackCoolingDown, true, FastAttackCoolDownTimer, 0.5f, DeltaTime);

	/* Debug */ if (GEngine && CanApplyDamage && false)GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Blue, TEXT("Damaging Enabled"));

	if (SR_TimerActive) SprintRollTimer += DeltaTime; // Timer for same button rolling and sprinting.
	if (SprintRollTimer >= SprintRollThresh) 
	{
		IsSprinting = true;
		CharMovComp->MaxWalkSpeed = 700.0f;
		OrientSpeed = 240.0f;
	}
	else {
		IsSprinting = false;
		CharMovComp->MaxWalkSpeed = DefaultMaxWalkSpeed;
		OrientSpeed = DefaultOrientSpeed;
	}
	
}

void APlayerCharacterV2::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Target", IE_Pressed, this, &APlayerCharacterV2::TargetPressedBind);
	PlayerInputComponent->BindAction("Target", IE_Released, this, &APlayerCharacterV2::TargetReleasedBind);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APlayerCharacterV2::JumpPressedBind);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &APlayerCharacterV2::JumpReleasedBind);
}

void APlayerCharacterV2::TargetPressedBind()
{
	CanOrientToStickInput = false;
	UseWalkMovementController = true;
	StrafeEnabled = true; // for animbp
	TargetingComponent->GetNewTarget();
}

void APlayerCharacterV2::TargetReleasedBind()
{
	CanOrientToStickInput = true;
	UseWalkMovementController = false;
	StrafeEnabled = false; // for animbp
	TargetingComponent->ClearTarget();
}

void APlayerCharacterV2::JumpPressedBind()
{
	SR_TimerActive = true;
}

void APlayerCharacterV2::JumpReleasedBind()
{
	SR_TimerActive = false;
	if (SprintRollTimer <= SprintRollThresh)
	{
		Roll();
	}
	SprintRollTimer = 0.0f;
}

void APlayerCharacterV2::Input1PressedBind()
{
	Input1Pressed = true;
	if (CanStartFastAttack && !FastAttackCoolingDown)
	{
		StartFastAttack();
		/* Debug */ if (GEngine && false)GEngine->AddOnScreenDebugMessage(-1, 4, FColor::Red, TEXT("StartedFast"));
	}
}

void APlayerCharacterV2::Input1ReleasedBind()
{
	Input1Pressed = false;
}

void APlayerCharacterV2::Input2PressedBind()
{
	Input2Pressed = true;
	if (CanStartStrongAttack && !StrongAttackCoolingDown) 
	{
		StartStrongAttack();
		/* Debug */ if (GEngine && false)GEngine->AddOnScreenDebugMessage(-1, 4, FColor::Red, TEXT("StartedStrong"));
	}
}

void APlayerCharacterV2::Input2ReleasedBind()
{
	Input2Pressed = false;
}

void APlayerCharacterV2::UniqueDeath()
{
	APlayerController* PCtrl = Cast<APlayerController>(GetController());
	if (PCtrl) DisableInput(PCtrl);
	GetMesh()->SetCollisionProfileName("Ragdoll");
	GetMesh()->SetSimulatePhysics(true);
}

void APlayerCharacterV2::Roll()
{
	if (!CanStartRoll) return;
	/* Debug */if (GEngine && false)GEngine->AddOnScreenDebugMessage(-1, 4, FColor::Red, TEXT("Rolled!"));

	// If player is giving movement input, play animation without start. Else just play the animation.
	if (MovementInput.Length() > 0.0f) PlayAnimMontage(RollAnimation, 1.0f, FName("Moving"));
	else PlayAnimMontage(RollAnimation);

	TargetingComponent->CanRotateOwner = false;
	InstantlyRotateToInput();
	OrientSpeed = 60.0f;
	CanTakeDamage = false;
	CanStartFastAttack = false;
	CanStartStrongAttack = false;
	CanStartRoll = false;
}

void APlayerCharacterV2::EndRoll()
{
	/* Debug */if (GEngine && false)GEngine->AddOnScreenDebugMessage(-1, 4, FColor::Red, TEXT("Roll Ended"));

	// Don't play the end off roll animation if player wants to move immediately.
	if (MovementInput.Length() > 0.0f) StopAnimMontage(RollAnimation);

	TargetingComponent->CanRotateOwner = true;
	OrientSpeed = DefaultOrientSpeed;
	CanTakeDamage = true;
	CanStartFastAttack = true;
	CanStartStrongAttack = true;
	CanStartRoll = true;
}

void APlayerCharacterV2::InstantlyRotateToInput()
{
	if (MovementInput.Length() > 0.0f)
	{
		FVector Direction = this->GetActorLocation() + FRotator(0.0f, GetControlRotation().Yaw, 0.0f).RotateVector(MovementInput);
		FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation(), Direction);
		this->SetActorRotation(Rotation);
	}
}

//////////////////////////////////////////////////////////
// ************ For combat combo logic **************** //
//////////////////////////////////////////////////////////



void APlayerCharacterV2::StartFastAttack()
{
	if (FastAttackAnimation) PlayAnimMontage(FastAttackAnimation, 1.0f, TEXT("Attack1"));
	CanStartFastAttack = false;
	CanStartStrongAttack = false;
}

void APlayerCharacterV2::StartStrongAttack()
{
	if (StrongAttackAnimation) PlayAnimMontage(StrongAttackAnimation, 1.0f, TEXT("Attack1"));
	CanStartStrongAttack = false;
	CanStartFastAttack = false;
}

// Executed by notifystate tick over some time, only one bool can be true.
void APlayerCharacterV2::CheckAttackPressed()
{
	if (Input1WasPressed)
	{
		ShouldContinueFastAttack = true;
		ShouldContinueStrongAttack = false;
	}
	else if (Input2WasPressed)
	{
		ShouldContinueFastAttack = false;
		ShouldContinueStrongAttack = true;
	}
}

void APlayerCharacterV2::ContinueAttack()
{
	if (ShouldContinueFastAttack)
	{
		if (GetMesh()->GetAnimInstance()->Montage_IsPlaying(StrongAttackAnimation))
		{
			StopAnimMontage(StrongAttackAnimation);
			ShouldContinueFastAttack = false;
			ShouldContinueStrongAttack = false;
			CurrentStrongAttackSectionCache = CurrentStrongAttackSection;
			TransToFastAttack();
		}
		else {
			ShouldContinueFastAttack = false;
			ShouldContinueStrongAttack = false;

			/* Debug */ if (GEngine && false) GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Cyan, TEXT("FastContinue"));
		}
	}
	else if (ShouldContinueStrongAttack)
	{
		if (GetMesh()->GetAnimInstance()->Montage_IsPlaying(FastAttackAnimation))
		{
			StopAnimMontage(FastAttackAnimation);
			ShouldContinueFastAttack = false;
			ShouldContinueStrongAttack = false;
			CurrentFastAttackSectionCache = CurrentFastAttackSection;
			TransToStrongAttack();
		}
		else {
			ShouldContinueFastAttack = false;
			ShouldContinueStrongAttack = false;

			
			/* Debug */ if (GEngine && false) GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Cyan, TEXT("StrongContinue"));
		}
	}
	else {
		if (GetMesh()->GetAnimInstance()->Montage_IsPlaying(FastAttackAnimation))
		{
			StopAnimMontage(FastAttackAnimation);
		}
		else if (GetMesh()->GetAnimInstance()->Montage_IsPlaying(StrongAttackAnimation))
		{
			StopAnimMontage(StrongAttackAnimation);
		}
		ShouldContinueFastAttack = false;
		ShouldContinueStrongAttack = false;
		EndAttack();
	}
}

void APlayerCharacterV2::TransToStrongAttack()
{
	/* Debug */ if (GEngine && false) GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Cyan, TEXT("StrongTrans"));

	if (StrongAttackAnimation)
	{
		if (CurrentFastAttackSectionCache == TEXT("Attack1") || CurrentFastAttackSectionCache == TEXT("Attack3"))
		{
			PlayAnimMontage(StrongAttackAnimation, 1.0f, TEXT("Attack2"));
		}
		else if (CurrentFastAttackSectionCache == TEXT("Attack2") || CurrentFastAttackSectionCache == TEXT("Attack4"))
		{
			PlayAnimMontage(StrongAttackAnimation, 1.0f, TEXT("Attack1"));
		}

	}
	CanStartFastAttack = false;
	CanStartStrongAttack = false;
}

void APlayerCharacterV2::TransToFastAttack()
{
	/* Debug */ if (GEngine && false) GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Cyan, TEXT("FastTrans"));

	if (FastAttackAnimation)
	{
		// Transition to different attacks depending on Transitioner's section (Attacks should flow together).
		if (CurrentStrongAttackSectionCache == TEXT("Attack1") || CurrentStrongAttackSectionCache == TEXT("Attack3"))
		{
			PlayAnimMontage(FastAttackAnimation, 1.0f, TEXT("Attack2"));
		}
		else if (CurrentStrongAttackSectionCache == TEXT("Attack2") || CurrentStrongAttackSectionCache == TEXT("Attack4"))
		{
			PlayAnimMontage(FastAttackAnimation, 1.0f, TEXT("Attack1"));
		}
	}

	CanStartFastAttack = false;
	CanStartStrongAttack = false;
}

// Called at end of montages so that attacks can be started again.
void APlayerCharacterV2::EndAttack()
{
	CanStartFastAttack = true;
	CanStartStrongAttack = true;
	FastAttackCoolingDown = true;
	StrongAttackCoolingDown = true;

	CanOrientToStickInput = true;

	/* Debug */ if (GEngine && false) GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Green, TEXT("AttackEnd"));
}
