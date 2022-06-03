// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"
#include "Components/CapsuleComponent.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	float mesh_loc = GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * -1.0f;
	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, mesh_loc));

}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseCharacter::Cooldown(bool & CoolingDown, float & CooldownTimer, float & CoolDownLength, float DeltaTime)
{
	if (CoolingDown)
	{
		CooldownTimer += DeltaTime;
	}
	if (CooldownTimer >= CoolDownLength)
	{
		CoolingDown = false;
		CooldownTimer = 0.0f;
	}
}

void ABaseCharacter::WasPressed(bool & WasPressed, bool & Pressed, bool & PressedCache)
{
	WasPressed = false;
	if (Pressed != PressedCache && Pressed == true) // Setting WasPressed to true if presssed in last frame.
	{
		WasPressed = true;
	}
	PressedCache = Pressed;
}

void ABaseCharacter::Death()
{
	UniqueDeath(); // Runs characters own unique death method. for example; players input is disabled and enemys AI is disabled.
	IsDead = true;
	Health = 0;
	
	if (DeathAnimation)
	{
		PlayAnimMontage(DeathAnimation); // Death montage should have an AnimNotify in it that fires RagdollChar when finished.
	}
}

void ABaseCharacter::UniqueDeath()
{
}

void ABaseCharacter::RagdollChar()
{
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("NoCollision"));
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationCustomMode); // Should disable anim bp
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->SetSimulatePhysics(true);
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

// Base implementation of health getting
int ABaseCharacter::GetMaxHealth()
{
	return MaxHealth;
}

int ABaseCharacter::GetHealth()
{
	return Health;
}

void ABaseCharacter::SetApplyDamageEnabled(bool NewDamage)
{
	CanApplyDamage = NewDamage;
}

void ABaseCharacter::SetCharacterRotationEnabled(bool NewRotate)
{
}

