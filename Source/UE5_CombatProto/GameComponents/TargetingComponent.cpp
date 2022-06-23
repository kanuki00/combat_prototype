// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetingComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "../Characters/V1/BaseCharacter.h"
#include "../Characters/BaseCharacterV2.h"

UTargetingComponent::UTargetingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	Owner = Cast<APawn>(GetOwner());
}

void UTargetingComponent::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void UTargetingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!IsDead)
	{
		// Determining targetable actors. Quite slow as is, but executing in tick is necessary to draw suggestion widget.
		AllTargets.Empty();
		AllVisibleTargets.Empty();
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), TargetedActorClass, AllTargets);
		if (AllTargets.Num() > 0)
		{
			for (int i = 0; i < AllTargets.Num(); i++)
			{
				if (
					ActorInView(AllTargets[i])																	// Check if in view
					&& !ActorOccluded(AllTargets[i])															// Check if not occluded by any objects
					&& ActorInRangeOfLocation(AllTargets[i], GetOwner()->GetActorLocation(), TargetingRange)	// Check if in range
					&& !(Cast<ABaseCharacterV2>(AllTargets[i])->IsDead)											// Check if alive
					)
				{
					AllVisibleTargets.Emplace(AllTargets[i]);
				}
			}
		}
		UpdateTargetingBiasLocation();
		SortActorsByDistanceToLocation(AllVisibleTargets, TargetingBiasLocation);



		if (IsTargeting)
		{
			SuggestedTarget = nullptr;

			// Rotating camera to target when targeting
			LookAtTarget(DeltaTime);
			RotateOwnerToTarget(DeltaTime);
		}
		else
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
	}
	else // if dead, make pointers null.
	{
		Target = nullptr;
		SuggestedTarget = nullptr;
	}
}

void UTargetingComponent::GetNewTarget()
{
	if (AllVisibleTargets.Num() > 0)
	{
		if(AllVisibleTargets[0]) Target = AllVisibleTargets[0];
		IsTargeting = true;
	}
}

void UTargetingComponent::ClearTarget()
{
	Target = nullptr;
	IsTargeting = false;
}

void UTargetingComponent::LookAtTarget(float DeltaTime)
{

	float rotspeed = 60.0f; // how fast the camera rotates.
	float play = 10.0f; // how many degrees camera has to be horizontally off-target to start rotating.
	float vertplay = 0.0f; // how many degrees camera has to be vertically off-target to start rotating.
	float vertoffset = 18.0f; // vertical offset,more positive number makes target appear higher above player character.

	if (Target != nullptr)
	{
		// Horizontal rotation

		FVector2D TargetGroundLoc = FVector2D(Target->GetActorLocation());
		FVector2D SelfGroundLoc = FVector2D(GetOwner()->GetActorLocation());
		FVector2D VToTargetGround = FVector2D(TargetGroundLoc - SelfGroundLoc).GetSafeNormal();

		FRotator ControlYawRot = FRotator(0.0f, Owner->GetControlRotation().Yaw, 0.0f);
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
			FRotator NewHozRot = FRotator(Owner->GetControlRotation().Pitch, Owner->GetControlRotation().Yaw + FMath::Clamp(rotspeed, 0.0f, -Hozlimit - play), Owner->GetControlRotation().Roll);
			Owner->GetController()->SetControlRotation(FMath::RInterpTo(Owner->GetControlRotation(), NewHozRot, DeltaTime, 5.0f));
		}
		else if (angle > play)
		{
			float Hozlimit = FMath::Clamp(angle, 0.0f, 180.0f);
			FRotator NewHozRot = FRotator(Owner->GetControlRotation().Pitch, Owner->GetControlRotation().Yaw + FMath::Clamp(-rotspeed, -Hozlimit + play, 0.0f), Owner->GetControlRotation().Roll);
			Owner->GetController()->SetControlRotation(FMath::RInterpTo(Owner->GetControlRotation(), NewHozRot, DeltaTime, 5.0f));
		}

		// Vertical rotation 

		FVector TargetLoc = Target->GetActorLocation();
		FVector VtoTarget = FVector(TargetLoc - Owner->GetActorLocation()).GetSafeNormal();
		FVector CharUpVec = UKismetMathLibrary::GetUpVector(Owner->GetActorRotation());
		FVector VertTargetPlaneN = UKismetMathLibrary::Cross_VectorVector(VtoTarget, CharUpVec).GetSafeNormal();
		FVector VtoTargetUpVec = UKismetMathLibrary::Cross_VectorVector(VertTargetPlaneN, VtoTarget).GetSafeNormal();

		FVector CamFwdVec = UKismetMathLibrary::GetForwardVector(UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayerCameraManager->GetCameraRotation());

		float VertLikeness = CamFwdVec.Dot(VtoTargetUpVec);
		float VertAngle = UKismetMathLibrary::DegAcos(VertLikeness) - 90.0f - vertoffset;

		if (VertAngle < -vertplay)
		{
			float vertlimit = FMath::Clamp(VertAngle, -180.0f, 0.0f);
			FRotator NewVertRot = FRotator(Owner->GetControlRotation().Pitch + FMath::Clamp(-rotspeed, vertlimit + vertplay, 0.0f), Owner->GetControlRotation().Yaw, Owner->GetControlRotation().Roll);
			Owner->GetController()->SetControlRotation(FMath::RInterpTo(Owner->GetControlRotation(), NewVertRot, DeltaTime, 5.0f));
		}
		else if (VertAngle > vertplay)
		{
			float vertlimit = FMath::Clamp(VertAngle, 0.0f, 180.0f);
			FRotator NewVertRot = FRotator(Owner->GetControlRotation().Pitch + FMath::Clamp(rotspeed, 0.0f, vertlimit - vertplay), Owner->GetControlRotation().Yaw, Owner->GetControlRotation().Roll);
			Owner->GetController()->SetControlRotation(FMath::RInterpTo(Owner->GetControlRotation(), NewVertRot, DeltaTime, 5.0f));
		}

		// Debug
		/*
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
		*/
	}
}

void UTargetingComponent::RotateOwnerToTarget(float DeltaTime)
{
	if (Target && Owner)
	{
		FVector TargetLocation = Target->GetActorLocation();
		FVector OwnerLocation = Owner->GetActorLocation();
		FRotator Rotation = FRotator(0.0f, UKismetMathLibrary::FindLookAtRotation(OwnerLocation, TargetLocation).Yaw, 0.0f);
		Owner->SetActorRotation(UKismetMathLibrary::RInterpTo_Constant(Owner->GetActorRotation(), Rotation, DeltaTime, 720.0f));
	}
}

bool UTargetingComponent::ActorInView(AActor* Actor)
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

bool UTargetingComponent::ActorOccluded(AActor* Actor)
{
	if (Actor)
	{
		FHitResult TraceResult;
		FVector Start = UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayerCameraManager->GetCameraLocation();
		FVector End = Actor->GetActorLocation();
		//GetWorld()->LineTraceSingleByChannel(TraceResult, Start, End, ECollisionChannel::ECC_Visibility);
		UKismetSystemLibrary::LineTraceSingle(GetWorld(), Start, End, ETraceTypeQuery::TraceTypeQuery1, true, AllTargets, EDrawDebugTrace::None, TraceResult, true);
		bool Result = TraceResult.bBlockingHit;

		return Result;
	}
	else
	{
		return false;
	}
}

bool UTargetingComponent::ActorInRangeOfLocation(AActor* Actor, FVector Location, float Range)
{
	return FVector::Dist(Actor->GetActorLocation(), Location) <= Range;
}

void UTargetingComponent::UpdateTargetingBiasLocation(float RayLength)
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
	/*
	if (MainCharacterDebug && MainCharacterDebugLevel == 1)
	{
		DrawDebugBox(GetWorld(), TargetingBiasLocation, FVector(20.0f, 20.0f, 20.0f), FColor::Red, false, -1.0f, 0, 2.0f);
	}
	*/
}

void UTargetingComponent::SortActorsByDistanceToLocation(TArray<AActor*>& Actors, FVector Location)
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


