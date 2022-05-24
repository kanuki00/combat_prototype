// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "CombatInterface.h"

// Sets default values
AWeapon::AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
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

	Ow_Imp_CI = GetOwner()->Implements<UCombatInterface>();

	//FString own = GetOwner()->GetName();
	//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, own);
	
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Weapon checks every frame what actors it is overlapping. Excludes owner from list.
	OverlappedActors.Empty();
	/*
	TArray<AActor*> OverlapsTemp;
	Collision->GetOverlappingActors(OverlapsTemp);
	if (OverlapsTemp.Num() > 0)
	{
		for (int i = 0; i < OverlapsTemp.Num(); i++)
		{
			if (OverlapsTemp[i] != GetOwner())
			{
				OverlappedActors.Emplace(OverlapsTemp[i]);
			}
		}

		if (OverlappedActors.Num() > 0)
		{
			FString a = OverlappedActors[0]->GetName();
			if (GEngine && false) GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, a);
		}
	}
	*/
}

void AWeapon::NotifyActorBeginOverlap(AActor* OtherActor)
{
	if (Ow_Imp_CI && OtherActor != GetOwner())
	{
		Cast<ICombatInterface>(GetOwner())->WeaponHit(OtherActor);
	}
}

