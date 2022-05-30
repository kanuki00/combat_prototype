// Copyright Kalevi Toivanen 2022.

#include "MainCharacter.h"
#include "Math/Vector.h"
#include "Math/Rotator.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

////////////////////////////////////////////////////////////////////////
// ******* Constructor, initializing components and settings ******** //
////////////////////////////////////////////////////////////////////////
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
	CameraBoom->TargetArmLength = 380.0f;
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 15.0f;

	CharacterMovement->JumpZVelocity = 1000.0f;
	CharacterMovement->GravityScale = 3.0f;

	Camera->bCameraMeshHiddenInGame = false;

	// Basecharacter
	// Initial health
	MaxHealth = 200;
	Health = 200;
}

////////////////////////////////////////////////////////////////////////
// ************* Begin Play, Called at start of game **************** //
////////////////////////////////////////////////////////////////////////
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
	RotateToInput(DeltaTime, 720.0f, PlayerHasRotationControl, IsTargeting);

	// Setting MovementInputStrength. Used mainly in animation blueprint via CharacterAnimation interface.
	MovementInputStrength = FMath::Clamp(MovementInputVector.Length(), 0.0f, 1.0f);

	// **** Was Pressed **** //
	WasPressed(Action1WasPressed, Action1Pressed, Action1PressedCache);
	WasPressed(Action2WasPressed, Action2Pressed, Action2PressedCache);
	
	// **** Cooldowns **** //
	Cooldown(FastAttackCoolingDown, FastAttackCooldownTimer, FastAttackCoolDownLength, DeltaTime);
	Cooldown(StrongAttackCoolingDown, StrongAttackCooldownTimer, StrongAttackCoolDownLength, DeltaTime);

	// Keeping track of what section each attack montage is in.
	CurrentFastAttackSection = GetMesh()->GetAnimInstance()->Montage_GetCurrentSection(FastAttack);
	CurrentStrongAttackSection = GetMesh()->GetAnimInstance()->Montage_GetCurrentSection(StrongAttack);

	// Determining targetable actors. Quite slow as is, but executing in tick is necessary to draw suggestion widget.
	AllTargets.Empty();
	AllVisibleTargets.Empty();
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), TargetedActorClass, AllTargets);
	if (AllTargets.Num() > 0)
	{
		for (int i = 0; i < AllTargets.Num(); i++)
		{
			if (
				ActorInView(AllTargets[i]) 
				&& !ActorOccluded(AllTargets[i]) 
				&& ActorInRangeOfLocation(AllTargets[i], this->GetActorLocation(), TargetingRange)
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
}

///////////////////////////////////////////////////////////////////////
// ********* Basic Character functions ******************************//
///////////////////////////////////////////////////////////////////////

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

void AMainCharacter::RotateToInput(float DeltaTime, float Rate, bool Enabled, bool Targeting)
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

	PlayerInputComponent->BindAction("Target", IE_Pressed, this, &AMainCharacter::TargetPressedBind);
	PlayerInputComponent->BindAction("Target", IE_Released, this, &AMainCharacter::TargetReleasedBind);
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
		if (GEngine && true && MainCharacterDebug && MainCharacterDebugLevel == 0) GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Red, TEXT("FastStarted"));
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
		if (GEngine && true && MainCharacterDebug && MainCharacterDebugLevel == 0) GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Red, TEXT("StrongStarted"));
	}
} 

void AMainCharacter::Action2ReleasedBind()
{Action2Pressed = false;}

void AMainCharacter::TargetPressedBind()
{
	TargetPressed = true;
	GetNewTarget();
}

void AMainCharacter::TargetReleasedBind()
{
	TargetPressed = false;
	ClearTarget();
}

////////////////////////////////////////////////////////////////////////
// *********************** Targeting ******************************** //
////////////////////////////////////////////////////////////////////////

void AMainCharacter::GetNewTarget()
{
	if (AllVisibleTargets.Num() > 0)
	{
		Target = AllVisibleTargets[0];
		IsTargeting = true;
	}
}

void AMainCharacter::ClearTarget()
{
	Target = nullptr;
	IsTargeting = false;
}

void AMainCharacter::LookAtTarget(bool Enabled, float DeltaTime)
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

bool AMainCharacter::ActorInView(AActor* Actor)
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

bool AMainCharacter::ActorOccluded(AActor* Actor)
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

void AMainCharacter::UpdateTargetingBiasLocation(float RayLength)
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

void AMainCharacter::SortActorsByDistanceToLocation(TArray<AActor*>& Actors, FVector Location)
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

bool AMainCharacter::ActorInRangeOfLocation(AActor* Actor, FVector Location, float Range)
{
	return FVector::Dist(Actor->GetActorLocation(), Location) <= Range;
}

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

void AMainCharacter::TransToStrongAttack()
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

void AMainCharacter::TransToFastAttack()
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
void AMainCharacter::EndAttack()
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

void AMainCharacter::SetCharacterRotationEnabled(bool NewRotate)
{
	PlayerHasRotationControl = NewRotate;
}

/////////////////////////////////////////////////////////////////
//*************** Interface implementation ********************//
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

float AMainCharacter::GetMovementInputStrength_Implementation()
{
	return MovementInputStrength;
}

FVector AMainCharacter::GetMovementInputDirection_Implementation()
{
	return MovementInputVector;
}
