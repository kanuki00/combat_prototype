// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "CombatInterface.h"

// Sets default values
AWeapon::AWeapon()
{
 	// Setting up components.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Actor Root"));
	RootComponent = Root;

	Collision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Collision"));
	Collision->SetupAttachment(RootComponent);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	Mesh->SetupAttachment(RootComponent);

	Mesh->SetCollisionProfileName(TEXT("NoCollision"));
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	SetOwner(GetInstigator());
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Weapon checks every frame what actors it is overlapping. Excludes owner from list.
	OverlappedActors.Empty();
}

void AWeapon::NotifyActorBeginOverlap(AActor* OtherActor)
{

}

