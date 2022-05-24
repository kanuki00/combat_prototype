// Copyright Kalevi Toivanen 2022.

#include "MainCharacter.h"
#include "Math/Vector.h"
#include "Math/Rotator.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AMainCharacter::AMainCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Setting up components, their attachments and initial locations.
	GetCapsuleComponent()->InitCapsuleSize(30.0f, 90.0f);
	GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	float mesh_loc = GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * -1.0f;
	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, mesh_loc));

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Boom"));
	CameraBoom->SetupAttachment(RootComponent);
	float boom_loc = GetCapsuleComponent()->GetScaledCapsuleHalfHeight()*1.0f - GetCapsuleComponent()->GetScaledCapsuleRadius();
	CameraBoom->SetRelativeLocation(FVector(0.0f, 0.0f, boom_loc));
	
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	// Other settings.
	bUseControllerRotationYaw = false;

	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->bInheritRoll = false;
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 15.0f;

	CharacterMovement->JumpZVelocity = 1000.0f;
	CharacterMovement->GravityScale = 3.0f;

	// Basecharacter
	// Initial health
	MaxHealth = 200;
	Health = 200;
}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Initial character rotation
	CharacterFacing = FRotator(0.0f, GetControlRotation().Yaw, 0.0f);
	
}

////////////////////////////////////////////////////////////////////////
// ************* Tick, Called every frame *************************** //
////////////////////////////////////////////////////////////////////////
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Movement(PlayerHasMovementControl);
	CameraMovement(DeltaTime, 20.0f, PlayerHasCameraControl);
	RotateToInput(DeltaTime, 720.0f, PlayerHasRotationControl);

	// Setting MovementInputStrength. Used mainly in animation blueprint via CharacterAnimation interface.
	MovementInputStrength = FMath::Clamp(MovementInputVector.Length(), 0.0f, 1.0f);

	// Setting Action1WasPressed to true if presssed in last frame.
	Action1WasPressed = false;
	if (Action1Pressed != Action1PressedCache && Action1Pressed == true)
	{
		Action1WasPressed = true;
	}
	Action1PressedCache = Action1Pressed;
	// Setting Action2WasPressed to true if presssed in last frame.
	Action2WasPressed = false;
	if (Action2Pressed != Action2PressedCache && Action2Pressed == true)
	{
		Action2WasPressed = true;
	}
	Action2PressedCache = Action2Pressed;
	
	// FastAttack cooldown
	if (FastAttackCoolingDown)
	{
		FastAttackCooldownTimer += DeltaTime;
	}
	if (FastAttackCooldownTimer >= 0.4f) // how long the cooldown is
	{
		FastAttackCoolingDown = false;
		FastAttackCooldownTimer = 0.0f;
	}
	// StrongAttackCooldown
	if (StrongAttackCoolingDown)
	{
		StrongAttackCooldownTimer += DeltaTime;
	}
	if (StrongAttackCooldownTimer >= 0.55f) // how long the cooldown is
	{
		StrongAttackCoolingDown = false;
		StrongAttackCooldownTimer = 0.0f;
	}

	// Keeping track of what section each attack montage is in.
	CurrentFastAttackSection = GetMesh()->GetAnimInstance()->Montage_GetCurrentSection(FastAttack);
	CurrentStrongAttackSection = GetMesh()->GetAnimInstance()->Montage_GetCurrentSection(StrongAttack);
}

///////////////////////////////////////////////////////////////////////
// ********* Basic Character functions ******************************//
///////////////////////////////////////////////////////////////////////

float AMainCharacter::GetMovementInputStrength_Implementation()
{
	return MovementInputStrength;
}



void AMainCharacter::Movement(bool Enabled)
{
	if (Enabled)
	{
		FVector RotatedInput = FRotator(0.0f, GetControlRotation().Yaw, 0.0f).RotateVector(MovementInputVector);
		FVector Direction = RotatedInput.GetSafeNormal();
		float Scale = FMath::Clamp(RotatedInput.Length(), 0.0f, 1.0f);
		AddMovementInput(Direction, Scale);
	}
}

void AMainCharacter::CameraMovement(float DeltaTime, float Smoothing, bool Enabled)
{
	if (Enabled)
	{
		PitchAmount = UKismetMathLibrary::FInterpTo(PitchAmount, CameraInputVector.Y, DeltaTime, Smoothing);
		YawAmount = UKismetMathLibrary::FInterpTo(YawAmount, CameraInputVector.X, DeltaTime, Smoothing);

		AddControllerPitchInput(PitchAmount);
		AddControllerYawInput(YawAmount);
	}
}

// Rotate character completely to latest movement input direction
void AMainCharacter::RotateToInput(float DeltaTime, float Rate, bool Enabled)
{
	if (Enabled)
	{
		if (MovementInputVector.Length() > 0.0f)
		{
			FVector Target = this->GetActorLocation() + FRotator(0.0f, GetControlRotation().Yaw, 0.0f).RotateVector(MovementInputVector);
			FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation(), Target);
			CharacterFacing = FRotator(0.0f, Rotation.Yaw, 0.0f);
		}
		SetActorRotation(FMath::RInterpConstantTo(this->GetActorRotation(), CharacterFacing, DeltaTime, 720.0f));
	}
}

/////////////////////////////////////////////////////////////
// ****** Called to bind functionality to input ***********//
/////////////////////////////////////////////////////////////
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis(MoveForwardName, this, &AMainCharacter::MoveForwardBind);
	PlayerInputComponent->BindAxis(MoveRightName, this, &AMainCharacter::MoveRightBind);

	PlayerInputComponent->BindAxis(LookUpName, this, &AMainCharacter::LookUpBind);
	PlayerInputComponent->BindAxis(LookRightName, this, &AMainCharacter::LookRightBind);

	PlayerInputComponent->BindAction("Input1", IE_Pressed, this, &AMainCharacter::Action1PressedBind);
	PlayerInputComponent->BindAction("Input1", IE_Released, this, &AMainCharacter::Action1ReleasedBind);

	PlayerInputComponent->BindAction("Input2", IE_Pressed, this, &AMainCharacter::Action2PressedBind);
	PlayerInputComponent->BindAction("Input2", IE_Released, this, &AMainCharacter::Action2ReleasedBind);
}

void AMainCharacter::MoveForwardBind(float Axis)
{MovementInputVector = FVector(Axis, MovementInputVector.Y, 0.0f);}

void AMainCharacter::MoveRightBind(float Axis)
{MovementInputVector = FVector(MovementInputVector.X, Axis, 0.0f);}

void AMainCharacter::LookUpBind(float Axis)
{CameraInputVector = FVector(CameraInputVector.X, Axis, 0.0f);}

void AMainCharacter::LookRightBind(float Axis)
{CameraInputVector = FVector(Axis, CameraInputVector.Y, 0.0f);}

void AMainCharacter::Action1PressedBind()
{
	Action1Pressed = true; 
	if(CanStartFastAttack && !FastAttackCoolingDown) // Action1 starts fast attack montage
	{
		StartFastAttack();
		// DEBUG
		if (GEngine && true && MainCharacterDebug) GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Red, TEXT("FastStarted"));
	}
}

void AMainCharacter::Action1ReleasedBind()
{Action1Pressed = false;}

void AMainCharacter::Action2PressedBind()
{
	Action2Pressed = true; 
	if (CanStartStrongAttack && !StrongAttackCoolingDown) // Action2 starts strong attack montage
	{
		StartStrongAttack();
		// DEBUG
		if (GEngine && true && MainCharacterDebug) GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Red, TEXT("StrongStarted"));
	}
} 

void AMainCharacter::Action2ReleasedBind()
{Action2Pressed = false;}

//////////////////////////////////////////////////////////
// ************ For combat combo logic **************** //
//////////////////////////////////////////////////////////

void AMainCharacter::StartFastAttack()
{
	if (FastAttack) PlayAnimMontage(FastAttack, 1.0f, TEXT("Attack1")); 
	CanStartFastAttack = false;
	CanStartStrongAttack = false;
}
void AMainCharacter::StartStrongAttack()
{
	if (StrongAttack) PlayAnimMontage(StrongAttack, 1.0f, TEXT("Attack1")); 
	CanStartStrongAttack = false;
	CanStartFastAttack = false;
}



// Executed by notifystate tick over some time, only one bool can be true.
/*
void AMainCharacter::CheckFastAttackPressed()
{
	if (Action1WasPressed)
	{
		ShouldContinueFastAttack = true;
		ShouldContinueStrongAttack = false;
	}
	else if (Action2WasPressed)
	{
		ShouldContinueFastAttack = false;
		ShouldContinueStrongAttack = true;
	}
}
void AMainCharacter::CheckStrongAttackPressed()
{
	if (Action2WasPressed)
	{
		ShouldContinueStrongAttack = true;
		ShouldContinueFastAttack = false;
	}
	else if (Action1WasPressed)
	{
		ShouldContinueStrongAttack = false;
		ShouldContinueFastAttack = true;
	}
}
*/
void AMainCharacter::CheckAttackPressed()
{
	if (Action1WasPressed)
	{
		ShouldContinueFastAttack = true;
		ShouldContinueStrongAttack = false;
	}
	else if (Action2WasPressed)
	{
		ShouldContinueFastAttack = false;
		ShouldContinueStrongAttack = true;
	}
}

/*
void AMainCharacter::ContinueFastAttack()
{
	//GetMesh()->GetAnimInstance()->Montage_IsPlaying(FastAttack);
	if (ShouldContinueFastAttack)
	{
		// DEBUG
		if (GEngine && true && MainCharacterDebug) GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Yellow, TEXT("FastContinue"));
		CanStartFastAttack = false; 
		CanStartStrongAttack = false; 

		ShouldContinueFastAttack = false;
	}
	else if (ShouldContinueStrongAttack)
	{
		if (FastAttack) StopAnimMontage(FastAttack);
		ShouldContinueFastAttack = false;
		ShouldContinueStrongAttack = false;

		CanStartStrongAttack = false;
		CanStartFastAttack = false;

		CurrentFastAttackSectionCache = CurrentFastAttackSection;
		TransToStrongAttack();
	}
	else
	{
		if(FastAttack) StopAnimMontage(FastAttack);
		ShouldContinueFastAttack = false;
		EndAttack();
	}
}
void AMainCharacter::ContinueStrongAttack()
{
	if (ShouldContinueStrongAttack)
	{
		// DEBUG
		if (GEngine && true && MainCharacterDebug) GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Yellow, TEXT("StrongContinue"));
		CanStartStrongAttack = false; // These have to be set to false, otherwise input bind will call StartStrongAttack()
		CanStartFastAttack = false; // ^

		ShouldContinueStrongAttack = false;
	}
	else if (ShouldContinueFastAttack)
	{
		if (StrongAttack) StopAnimMontage(StrongAttack);
		ShouldContinueStrongAttack = false;
		ShouldContinueFastAttack = false;

		CanStartFastAttack = false;
		CanStartStrongAttack = false;

		CurrentStrongAttackSectionCache = CurrentStrongAttackSection;
		TransToFastAttack();
	}
	else
	{
		if(StrongAttack) StopAnimMontage(StrongAttack);
		ShouldContinueStrongAttack = false;
		EndAttack();
	}
}
*/

void AMainCharacter::ContinueAttack()
{
	if (ShouldContinueFastAttack)
	{
		if (GetMesh()->GetAnimInstance()->Montage_IsPlaying(StrongAttack))
		{
			StopAnimMontage(StrongAttack);
			ShouldContinueFastAttack = false;
			ShouldContinueStrongAttack = false;
			CurrentStrongAttackSectionCache = CurrentStrongAttackSection;
			TransToFastAttack();
		}
		else {
			ShouldContinueFastAttack = false;
			ShouldContinueStrongAttack = false;
		}
	}
	else if (ShouldContinueStrongAttack)
	{
		if (GetMesh()->GetAnimInstance()->Montage_IsPlaying(FastAttack))
		{
			StopAnimMontage(FastAttack);
			ShouldContinueFastAttack = false;
			ShouldContinueStrongAttack = false;
			CurrentFastAttackSectionCache = CurrentFastAttackSection;
			TransToStrongAttack();
		}
		else {
			ShouldContinueFastAttack = false;
			ShouldContinueStrongAttack = false;
		}
	}
	else {
		if (GetMesh()->GetAnimInstance()->Montage_IsPlaying(FastAttack))
		{
			StopAnimMontage(FastAttack);
		}
		else if (GetMesh()->GetAnimInstance()->Montage_IsPlaying(StrongAttack))
		{
			StopAnimMontage(StrongAttack);
		}
		ShouldContinueFastAttack = false;
		ShouldContinueStrongAttack = false;
		EndAttack();
	}
}

void AMainCharacter::TransToStrongAttack()
{
	// DEBUG
	if (GEngine && true && MainCharacterDebug) GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Cyan, TEXT("StrongTrans"));

	if (StrongAttack)
	{
		if (CurrentFastAttackSectionCache == TEXT("Attack1") || CurrentFastAttackSectionCache == TEXT("Attack3"))
		{
			PlayAnimMontage(StrongAttack, 1.0f, TEXT("Attack2"));
		}
		else if (CurrentFastAttackSectionCache == TEXT("Attack2") || CurrentFastAttackSectionCache == TEXT("Attack4"))
		{
			PlayAnimMontage(StrongAttack, 1.0f, TEXT("Attack1"));
		}
		
	}
	CanStartFastAttack = false;
	CanStartStrongAttack = false;
}

void AMainCharacter::TransToFastAttack()
{
	// DEBUG
	if (GEngine && true && MainCharacterDebug) GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Cyan, TEXT("FastTrans"));
	//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Cyan, CurrentStrongAttackSectionCache.ToString());

	if (FastAttack) 
	{
		// Transition to different attacks depending on Transitioner's section (Attacks should flow together).
		if (CurrentStrongAttackSectionCache == TEXT("Attack1") || CurrentStrongAttackSectionCache == TEXT("Attack3"))
		{
			PlayAnimMontage(FastAttack, 1.0f, TEXT("Attack2"));
		}
		else if (CurrentStrongAttackSectionCache == TEXT("Attack2") || CurrentStrongAttackSectionCache == TEXT("Attack4"))
		{
			PlayAnimMontage(FastAttack, 1.0f, TEXT("Attack1"));
		}
	}

	CanStartFastAttack = false;
	CanStartStrongAttack = false;
}

// Called at end of montages so that attacks can be started again.
void AMainCharacter::EndAttack()
{
	if (GEngine && true && MainCharacterDebug) GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Green, TEXT("AttackEnd"));
	CanStartFastAttack = true;
	CanStartStrongAttack = true;
	FastAttackCoolingDown = true; 
	StrongAttackCoolingDown = true;

	PlayerHasRotationControl = true;
}

/////////////////////////////////////////////////////////////////

void AMainCharacter::SetCharacterRotationEnabled(bool NewRotate)
{
	PlayerHasRotationControl = NewRotate;
}

/////////////////////////////////////////////////////////////////
//*********** Combat Interface implementation *****************//
/////////////////////////////////////////////////////////////////

void AMainCharacter::WeaponHit(AActor* HitActor)
{
	if (CanApplyDamage)
	{
		FString msg = HitActor->GetName();
		if (GEngine && MainCharacterDebug) GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, msg);

		// Applying damage
		FDamageEvent de;
		HitActor->TakeDamage(1.0f, de, GetController(), this);
	}
}
