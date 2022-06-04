// Copyright Kalevi Toivanen 2022.

#include "MainCharacter.h"
#include "Math/Vector.h"
#include "Math/Rotator.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"

////////////////////////////////////////////////////////////////////////
// ******* Constructor, initializing components and settings ******** //
////////////////////////////////////////////////////////////////////////
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Setting up components, their attachments and initial locations.
	GetCapsuleComponent()->InitCapsuleSize(30.0f, 90.0f);

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
	CameraBoom->TargetArmLength = 380.0f;
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 15.0f;

	CharacterMovement->JumpZVelocity = 1000.0f;
	CharacterMovement->GravityScale = 3.0f;

	Camera->bCameraMeshHiddenInGame = false;

	// Initial health
	MaxHealth = 200;
	Health = 200;

	DamageStrength = 35;
}

////////////////////////////////////////////////////////////////////////
// ************* Begin Play, Called at start of game **************** //
////////////////////////////////////////////////////////////////////////
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Initial character rotation
	CharacterFacing = FRotator(0.0f, GetControlRotation().Yaw, 0.0f);

}

////////////////////////////////////////////////////////////////////////
// ************* Tick, Called every frame *************************** //
////////////////////////////////////////////////////////////////////////
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsDead)
	{
		Movement(PlayerHasMovementControl);
		CameraMovement(DeltaTime, 20.0f, PlayerHasCameraControl);
		RotateToInput(DeltaTime, RotationSpeed, PlayerHasRotationControl, IsTargeting && !IsRolling);
		RotateCameraToMovement(!IsTargeting);
	}

	// Setting MovementInputStrength. Used mainly in animation blueprint via CharacterAnimation interface.
	MovementInputStrength = FMath::Clamp(MovementInputVector.Length(), 0.0f, 1.0f);

	// **** Was Pressed **** //
	WasPressed(Action1WasPressed, Action1Pressed, Action1PressedCache);
	WasPressed(Action2WasPressed, Action2Pressed, Action2PressedCache);
	
	// **** Cooldowns **** //
	Cooldown(FastAttackCoolingDown, FastAttackCooldownTimer, FastAttackCoolDownLength, DeltaTime);
	Cooldown(StrongAttackCoolingDown, StrongAttackCooldownTimer, StrongAttackCoolDownLength, DeltaTime);

	// Keeping track of what section each attack montage is in.
	if (!IsDead) {
		CurrentFastAttackSection = GetMesh()->GetAnimInstance()->Montage_GetCurrentSection(FastAttack);
		CurrentStrongAttackSection = GetMesh()->GetAnimInstance()->Montage_GetCurrentSection(StrongAttack);
	}
	

	// Determining targetable actors. Quite slow as is, but executing in tick is necessary to draw suggestion widget.
	AllTargets.Empty();
	AllVisibleTargets.Empty();
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), TargetedActorClass, AllTargets);
	if (AllTargets.Num() > 0)
	{
		for (int i = 0; i < AllTargets.Num(); i++)
		{
			if (
				ActorInView(AllTargets[i])																// Check is in view
				&& !ActorOccluded(AllTargets[i])														// Check if not occluded by any objects
				&& ActorInRangeOfLocation(AllTargets[i], this->GetActorLocation(), TargetingRange)		// Check if in range
				&& !(Cast<ABaseCharacter>(AllTargets[i])->IsDead)										// Check if alive
				)
			{
				AllVisibleTargets.Emplace(AllTargets[i]);
			}
		}
	}
	UpdateTargetingBiasLocation();
	SortActorsByDistanceToLocation(AllVisibleTargets, TargetingBiasLocation);
	if (!IsTargeting)
	{
		if (AllVisibleTargets.Num() > 0)
		{
			SuggestedTarget = AllVisibleTargets[0];
		}
		else
		{
			SuggestedTarget = nullptr;
		}
	}
	else 
	{
		SuggestedTarget = nullptr;
	}

	// Rotating camera to target when targeting
	LookAtTarget(TargetPressed, DeltaTime);

	// Determining if Input3 was tapped or is being held

	if (Input3Pressed == true)
	{
		Input3PressTimer += DeltaTime;
		if (Input3PressTimer > Input3TapThresh)
		{
			Input3Held = true;
		}
	}
	else {
		Input3Held = false;
	}

	// Setting sprint speed
	if (Input3Held && !IsTargeting) { CharacterMovement->MaxWalkSpeed = 800.0f; IsSprinting = true;
	}
	else { CharacterMovement->MaxWalkSpeed = 600.0f; IsSprinting = false; } // Such beautifully compact code! thx brackets.

	// Debug
	//if (GEngine && IsSprinting) GEngine->AddOnScreenDebugMessage(20, 0.1, FColor::Yellow, TEXT("Should be sprinting"));
}

///////////////////////////////////////////////////////////////////////
// ********* Basic Character functions ******************************//
///////////////////////////////////////////////////////////////////////

void APlayerCharacter::Movement(bool Enabled)
{
	if (Enabled)
	{
		FVector RotatedInput = FRotator(0.0f, GetControlRotation().Yaw, 0.0f).RotateVector(MovementInputVector);
		FVector Direction = RotatedInput.GetSafeNormal();
		float Scale = FMath::Clamp(RotatedInput.Length(), 0.0f, 1.0f);
		AddMovementInput(Direction, Scale);

		// DEBUG
		//if (GEngine) GEngine->AddOnScreenDebugMessage(1, 0.1, FColor::Yellow, MovementInputVector.ToString());
		//if (GEngine) GEngine->AddOnScreenDebugMessage(2, 0.1, FColor::Orange, CameraInputVector.ToString());

	}
}

void APlayerCharacter::CameraMovement(float DeltaTime, float Smoothing, bool Enabled)
{
	if (Enabled)
	{
		PitchAmount = UKismetMathLibrary::FInterpTo(PitchAmount, CameraInputVector.Y, DeltaTime, Smoothing);
		YawAmount = UKismetMathLibrary::FInterpTo(YawAmount, CameraInputVector.X, DeltaTime, Smoothing);

		AddControllerPitchInput(PitchAmount);
		AddControllerYawInput(YawAmount);
	}
}

void APlayerCharacter::RotateCameraToMovement(bool Enabled)
{
	if (Enabled && CameraInputVector.Length() == 0.0f)
	{
		AddControllerYawInput(MovementInputVector.Y*0.5f);
	}
}

void APlayerCharacter::RotateToInput(float DeltaTime, float Rate, bool Enabled, bool Targeting)
{
	if (Enabled)
	{
		if (Targeting && Target)
		{
			// rotating to targeted actor
			FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation(), Target->GetActorLocation());
			CharacterFacing = FRotator(0.0f, Rotation.Yaw, 0.0f);
		}
		else if (MovementInputVector.Length() > 0.0f)
		{
			// rotating to movement input
			FVector Direction = this->GetActorLocation() + FRotator(0.0f, GetControlRotation().Yaw, 0.0f).RotateVector(MovementInputVector);
			FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation(), Direction);
			CharacterFacing = FRotator(0.0f, Rotation.Yaw, 0.0f);
		}
		SetActorRotation(FMath::RInterpConstantTo(this->GetActorRotation(), CharacterFacing, DeltaTime, RotationSpeed));
	}
}

void APlayerCharacter::InstantlyRotateToInput()
{
	if (MovementInputVector.Length() > 0.0f)
	{
		FVector Direction = this->GetActorLocation() + FRotator(0.0f, GetControlRotation().Yaw, 0.0f).RotateVector(MovementInputVector);
		FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation(), Direction);
		this->SetActorRotation(Rotation);
	}
}

void APlayerCharacter::StartRoll()
{
	//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 4, FColor::Yellow, TEXT("Tapped!"));
	InstantlyRotateToInput();
	if (RollAnimation && CanStartRoll)
	{
		CanStartRoll = false;
		IsRolling = true;
		RotationSpeed = 180.0f;
		if (MovementInputStrength > 0.0f)
		{
			PlayAnimMontage(RollAnimation, 1.0f, FName("Moving"));
		}
		else
		{
			PlayAnimMontage(RollAnimation, 1.0f, FName("Standing"));
		}

	}
}

void APlayerCharacter::EndRoll()
{
	IsRolling = false;
	CanStartRoll = true;
	RotationSpeed = DefaultRotationSpeed;
	if (MovementInputStrength > 0.0f) // Stopping roll anim prematurely to keep movement velocity.
	{
		StopAnimMontage(RollAnimation);
	}
}

void APlayerCharacter::UniqueDeath()
{
	DisableInput(Cast<APlayerController>(GetController()));
}

/////////////////////////////////////////////////////////////
// ****** Called to bind functionality to input ***********//
/////////////////////////////////////////////////////////////
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("Move_Forward_Backward", this, &APlayerCharacter::MoveForwardBind);
	PlayerInputComponent->BindAxis("Move_Right_Left", this, &APlayerCharacter::MoveRightBind);

	PlayerInputComponent->BindAxis("Look_Up_Down", this, &APlayerCharacter::LookUpBind);
	PlayerInputComponent->BindAxis("Look_Right_Left", this, &APlayerCharacter::LookRightBind);

	PlayerInputComponent->BindAction("Input1", IE_Pressed, this, &APlayerCharacter::Action1PressedBind);
	PlayerInputComponent->BindAction("Input1", IE_Released, this, &APlayerCharacter::Action1ReleasedBind);

	PlayerInputComponent->BindAction("Input2", IE_Pressed, this, &APlayerCharacter::Action2PressedBind);
	PlayerInputComponent->BindAction("Input2", IE_Released, this, &APlayerCharacter::Action2ReleasedBind);

	PlayerInputComponent->BindAction("Input3", IE_Pressed, this, &APlayerCharacter::Input3PressedBind);
	PlayerInputComponent->BindAction("Input3", IE_Released, this, &APlayerCharacter::Input3ReleasedBind);

	PlayerInputComponent->BindAction("Target", IE_Pressed, this, &APlayerCharacter::TargetPressedBind);
	PlayerInputComponent->BindAction("Target", IE_Released, this, &APlayerCharacter::TargetReleasedBind);


}

void APlayerCharacter::MoveForwardBind(float Axis)
{
	//if (GEngine) GEngine->AddOnScreenDebugMessage(3, 0.1, FColor::Yellow, FString::SanitizeFloat(Axis));
	MovementInputVector = FVector(Axis, MovementInputVector.Y, 0.0f);
}

void APlayerCharacter::MoveRightBind(float Axis)
{
	MovementInputVector = FVector(MovementInputVector.X, Axis, 0.0f);
}

void APlayerCharacter::LookUpBind(float Axis)
{CameraInputVector = FVector(CameraInputVector.X, Axis, 0.0f);}

void APlayerCharacter::LookRightBind(float Axis)
{CameraInputVector = FVector(Axis, CameraInputVector.Y, 0.0f);}

void APlayerCharacter::Action1PressedBind()
{
	Action1Pressed = true; 
	if(CanStartFastAttack && !FastAttackCoolingDown && !IsRolling) // Action1 starts fast attack montage
	{
		StartFastAttack();
		// DEBUG
		if (GEngine && true && MainCharacterDebug && MainCharacterDebugLevel == 0) GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Red, TEXT("FastStarted"));
	}
}

void APlayerCharacter::Action1ReleasedBind()
{Action1Pressed = false;}

void APlayerCharacter::Action2PressedBind()
{
	Action2Pressed = true; 
	if (CanStartStrongAttack && !StrongAttackCoolingDown && !IsRolling) // Action2 starts strong attack montage
	{
		StartStrongAttack();
		// DEBUG
		if (GEngine && true && MainCharacterDebug && MainCharacterDebugLevel == 0) GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Red, TEXT("StrongStarted"));
	}
} 

void APlayerCharacter::Action2ReleasedBind()
{Action2Pressed = false;}

void APlayerCharacter::Input3PressedBind()
{
	Input3Pressed = true;
}

void APlayerCharacter::Input3ReleasedBind()
{
	Input3Pressed = false;
	if (Input3PressTimer <= Input3TapThresh)
	{
		Input3Tapped();
	}
	Input3PressTimer = 0.0f;
}

void APlayerCharacter::Input3Tapped()
{
	StartRoll();
}

void APlayerCharacter::TargetPressedBind()
{
	TargetPressed = true;
	GetNewTarget();
}

void APlayerCharacter::TargetReleasedBind()
{
	TargetPressed = false;
	ClearTarget();
}

////////////////////////////////////////////////////////////////////////
// *********************** Targeting ******************************** //
////////////////////////////////////////////////////////////////////////

void APlayerCharacter::GetNewTarget()
{
	if (AllVisibleTargets.Num() > 0)
	{
		Target = AllVisibleTargets[0];
		IsTargeting = true;
	}
}

void APlayerCharacter::ClearTarget()
{
	Target = nullptr;
	IsTargeting = false;
}

void APlayerCharacter::LookAtTarget(bool Enabled, float DeltaTime)
{

	float rotspeed = 60.0f; // how fast the camera rotates.
	float play = 10.0f; // how many degrees camera has to be horizontally off-target to start rotating.
	float vertplay = 0.0f; // how many degrees camera has to be vertically off-target to start rotating.
	float vertoffset = 18.0f; // vertical offset,more positive number makes target appear higher above player character.

	if (Enabled && Target != nullptr)
	{
		// Horizontal rotation

		FVector2D TargetGroundLoc = FVector2D(Target->GetActorLocation());
		FVector2D SelfGroundLoc = FVector2D(this->GetActorLocation());
		FVector2D VToTargetGround = FVector2D(TargetGroundLoc - SelfGroundLoc).GetSafeNormal();

		FRotator ControlYawRot = FRotator(0.0f, GetControlRotation().Yaw, 0.0f);
		FVector2D FV = FVector2D(UKismetMathLibrary::GetForwardVector(ControlYawRot));
		FVector2D RV = FVector2D(UKismetMathLibrary::GetRightVector(ControlYawRot));

		float LikenessF = FV.Dot(VToTargetGround);
		float LikenessR = RV.Dot(VToTargetGround);

		float angle = UKismetMathLibrary::DegAcos(LikenessR) - 90.0f;
		if (LikenessF < 0.0f && angle > 0.0f)
		{
			angle += 90.0f;
		}
		else if (LikenessF < 0.0f && angle < 0.0f)
		{
			angle -= 90.0f;
		}

		if (angle < -play)
		{
			float Hozlimit = FMath::Clamp(angle, -180.0f, 0.0f);
			FRotator NewHozRot = FRotator(GetControlRotation().Pitch, GetControlRotation().Yaw + FMath::Clamp(rotspeed, 0.0f, -Hozlimit - play), GetControlRotation().Roll);
			GetController()->SetControlRotation(FMath::RInterpTo(GetControlRotation(), NewHozRot, DeltaTime, 5.0f));
		}
		else if (angle > play)
		{
			float Hozlimit = FMath::Clamp(angle, 0.0f, 180.0f);
			FRotator NewHozRot = FRotator(GetControlRotation().Pitch, GetControlRotation().Yaw + FMath::Clamp(-rotspeed, -Hozlimit + play, 0.0f), GetControlRotation().Roll);
			GetController()->SetControlRotation(FMath::RInterpTo(GetControlRotation(), NewHozRot, DeltaTime, 5.0f));
		}

		// Vertical rotation 

		FVector TargetLoc = Target->GetActorLocation();
		FVector VtoTarget = FVector(TargetLoc - this->GetActorLocation()).GetSafeNormal();
		FVector CharUpVec = UKismetMathLibrary::GetUpVector(this->GetActorRotation());
		FVector VertTargetPlaneN = UKismetMathLibrary::Cross_VectorVector(VtoTarget, CharUpVec).GetSafeNormal();
		FVector VtoTargetUpVec = UKismetMathLibrary::Cross_VectorVector(VertTargetPlaneN, VtoTarget).GetSafeNormal();

		FVector CamFwdVec = UKismetMathLibrary::GetForwardVector(UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayerCameraManager->GetCameraRotation());

		float VertLikeness = CamFwdVec.Dot(VtoTargetUpVec);
		float VertAngle = UKismetMathLibrary::DegAcos(VertLikeness) - 90.0f - vertoffset;

		if (VertAngle < -vertplay)
		{
			float vertlimit = FMath::Clamp(VertAngle, -180.0f, 0.0f);
			FRotator NewVertRot = FRotator(GetControlRotation().Pitch + FMath::Clamp(-rotspeed, vertlimit + vertplay, 0.0f), GetControlRotation().Yaw, GetControlRotation().Roll);
			GetController()->SetControlRotation(FMath::RInterpTo(GetControlRotation(), NewVertRot, DeltaTime, 5.0f));
		}
		else if (VertAngle > vertplay)
		{
			float vertlimit = FMath::Clamp(VertAngle, 0.0f, 180.0f);
			FRotator NewVertRot = FRotator(GetControlRotation().Pitch + FMath::Clamp(rotspeed, 0.0f, vertlimit - vertplay), GetControlRotation().Yaw, GetControlRotation().Roll);
			GetController()->SetControlRotation(FMath::RInterpTo(GetControlRotation(), NewVertRot, DeltaTime, 5.0f));
		}

		// Debug

		if (GEngine && MainCharacterDebug && MainCharacterDebugLevel == 1) {
			FVector FeetLoc = this->GetActorLocation() + FVector(0.0f, 0.0f, -(GetCapsuleComponent()->GetScaledCapsuleHalfHeight()));
			DrawDebugLine(GetWorld(), FeetLoc, FeetLoc + FVector(VToTargetGround.X, VToTargetGround.Y, 0.0f) * 200.0f, FColor::Green, false, -1.0f, 0, 2.0f);
			DrawDebugLine(GetWorld(), FeetLoc, FeetLoc + FVector(RV.X, RV.Y, 0.0f) * 200.0f, FColor::Cyan, false, -1.0f, 0, 2.0f);
			DrawDebugLine(GetWorld(), FeetLoc, FeetLoc + FVector(FV.X, FV.Y, 0.0f) * 200.0f, FColor::Orange, false, -1.0f, 0, 2.0f);

			DrawDebugLine(GetWorld(), this->GetActorLocation(), this->GetActorLocation() + VtoTarget * 200.0f, FColor::Blue, false, -1.0f, 0, 2.0f);
			DrawDebugLine(GetWorld(), this->GetActorLocation(), this->GetActorLocation() + CharUpVec * 200.0f, FColor::Yellow, false, -1.0f, 0, 2.0f);
			DrawDebugLine(GetWorld(), this->GetActorLocation(), this->GetActorLocation() + VertTargetPlaneN * 200.0f, FColor::Yellow, false, -1.0f, 0, 2.0f);
			DrawDebugLine(GetWorld(), this->GetActorLocation(), this->GetActorLocation() + VtoTargetUpVec * 200.0f, FColor::Purple, false, -1.0f, 0, 2.0f);
			DrawDebugLine(GetWorld(), this->GetActorLocation(), this->GetActorLocation() + CamFwdVec * 200.0f, FColor::Red, false, -1.0f, 0, 2.0f);

			GEngine->AddOnScreenDebugMessage(3, 1, FColor::Orange, FString::SanitizeFloat(angle));
			GEngine->AddOnScreenDebugMessage(1, 1, FColor::Red, FString::SanitizeFloat(VertAngle));
		}
	}
}

bool APlayerCharacter::ActorInView(AActor* Actor)
{
	bool IsOnScreen = false;
	if (Actor)
	{
		FVector2D ScreenSize;
		if (GEngine && GEngine->GameViewport)
		{
			GEngine->GameViewport->GetViewportSize(ScreenSize);
		}
		FVector2D ActorScreenPosition;
		bool SuccessfullProjection = UGameplayStatics::ProjectWorldToScreen(UGameplayStatics::GetPlayerController(GetWorld(), 0), Actor->GetActorLocation(), ActorScreenPosition);

		if (SuccessfullProjection && ActorScreenPosition.X > 0.0f && ActorScreenPosition.X < ScreenSize.X && ActorScreenPosition.Y > 0.0f && ActorScreenPosition.Y < ScreenSize.Y)
		{
			IsOnScreen = true;
		}
		else {
			IsOnScreen = false;
		}
		// Debug
		//if (GEngine && IsOnScreen) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Purple, ActorScreenPosition.ToString());
		//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, ScreenSize.ToString());
	}
	return IsOnScreen;
}

bool APlayerCharacter::ActorOccluded(AActor* Actor)
{
	if (Actor)
	{
		FHitResult TraceResult;
		FVector Start = UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayerCameraManager->GetCameraLocation();
		FVector End = Actor->GetActorLocation();
		GetWorld()->LineTraceSingleByChannel(TraceResult, Start, End, ECollisionChannel::ECC_Visibility);
		bool Result = TraceResult.bBlockingHit;

		return Result;
	}
	else
	{
		return false;
	}
}

void APlayerCharacter::UpdateTargetingBiasLocation(float RayLength)
{
	FHitResult TraceResult;
	FVector CameraFwdVec = UKismetMathLibrary::GetForwardVector(UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayerCameraManager->GetCameraRotation());
	//FVector CameraUpVec = UKismetMathLibrary::GetUpVector(UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayerCameraManager->GetCameraRotation());
	FVector CameraRVec = UKismetMathLibrary::GetRightVector(UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayerCameraManager->GetCameraRotation());

	FVector TraceStart = UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayerCameraManager->GetCameraLocation();
	FVector TraceEnd = FVector(TraceStart + CameraFwdVec * RayLength).RotateAngleAxis(-6.0f, CameraRVec);
	// Tracing a line form a half metre above camera in camera's forward direction.
	GetWorld()->LineTraceSingleByChannel(TraceResult, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility);
	TargetingBiasLocation = TraceResult.ImpactPoint;

	//Debug
	if (MainCharacterDebug && MainCharacterDebugLevel == 1)
	{
		DrawDebugBox(GetWorld(), TargetingBiasLocation, FVector(20.0f, 20.0f, 20.0f), FColor::Red, false, -1.0f, 0, 2.0f);
	}
}

void APlayerCharacter::SortActorsByDistanceToLocation(TArray<AActor*>& Actors, FVector Location)
{
	for (int i = 0; i < Actors.Num(); i++)
	{
		for (int o = 0; o < Actors.Num() - 1; o++)
		{
			if (FVector::Dist(Actors[o]->GetActorLocation(), Location) > FVector::Dist(Actors[o + 1]->GetActorLocation(), Location))
			{
				Actors.Swap(o, o + 1);
			}
		}
	}
}

bool APlayerCharacter::ActorInRangeOfLocation(AActor* Actor, FVector Location, float Range)
{
	return FVector::Dist(Actor->GetActorLocation(), Location) <= Range;
}

//////////////////////////////////////////////////////////
// ************ For combat combo logic **************** //
//////////////////////////////////////////////////////////

void APlayerCharacter::StartFastAttack()
{
		if (FastAttack) PlayAnimMontage(FastAttack, 1.0f, TEXT("Attack1"));
		CanStartFastAttack = false;
		CanStartStrongAttack = false;
}

void APlayerCharacter::StartStrongAttack()
{
	if (StrongAttack) PlayAnimMontage(StrongAttack, 1.0f, TEXT("Attack1")); 
	CanStartStrongAttack = false;
	CanStartFastAttack = false;
}

// Executed by notifystate tick over some time, only one bool can be true.
void APlayerCharacter::CheckAttackPressed()
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

void APlayerCharacter::ContinueAttack()
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

			// Debug 
			if (GEngine && true && MainCharacterDebug && MainCharacterDebugLevel == 0) GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Cyan, TEXT("FastContinue"));
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

			// Debug 
			if (GEngine && true && MainCharacterDebug && MainCharacterDebugLevel == 0) GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Cyan, TEXT("StrongContinue"));
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

void APlayerCharacter::TransToStrongAttack()
{
	// DEBUG
	if (GEngine && true && MainCharacterDebug && MainCharacterDebugLevel == 0) GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Cyan, TEXT("StrongTrans"));

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

void APlayerCharacter::TransToFastAttack()
{
	// DEBUG
	if (GEngine && true && MainCharacterDebug && MainCharacterDebugLevel == 0) GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Cyan, TEXT("FastTrans"));

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
void APlayerCharacter::EndAttack()
{
	CanStartFastAttack = true;
	CanStartStrongAttack = true;
	FastAttackCoolingDown = true; 
	StrongAttackCoolingDown = true;

	PlayerHasRotationControl = true;

	// Debug 
	if (GEngine && true && MainCharacterDebug && MainCharacterDebugLevel == 0) GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Green, TEXT("AttackEnd"));
}

/////////////////////////////////////////////////////////////////
//*************** BaseCharacter Implementations ***************//
/////////////////////////////////////////////////////////////////

void APlayerCharacter::SetCharacterRotationEnabled(bool NewRotate)
{
	PlayerHasRotationControl = NewRotate;
}

/////////////////////////////////////////////////////////////////
//*************** Interface implementation ********************//
/////////////////////////////////////////////////////////////////

/*
void APlayerCharacter::WeaponHit(AActor* HitActor)
{
	// This is already implemented in baase character, consider removing
	if (CanApplyDamage)
	{
		FString msg = HitActor->GetName();
		if (GEngine && MainCharacterDebug) GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, msg);

		// Applying damage
		FDamageEvent de;
		HitActor->TakeDamage(1.0f, de, GetController(), this);
	}
}
*/
float APlayerCharacter::GetMovementInputStrength_Implementation()
{
	return MovementInputStrength;
}

FVector APlayerCharacter::GetMovementInputDirection_Implementation()
{
	return MovementInputVector;
}
