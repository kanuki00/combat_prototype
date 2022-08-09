// 


#include "Weapon.h"
#include "Characters/BaseCharacterV2.h"
#include "Components/CapsuleComponent.h"

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
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	/*
	// Weapon checks every frame what actors it is overlapping. Excludes owner from list.
	RawOverlappedActors.Empty();
	OverlappedActors.Empty();
	
	GetOverlappingActors(RawOverlappedActors);
	if (RawOverlappedActors.Num() == 0) return;
	for(AActor* A : RawOverlappedActors)
	{
		if (A != Owner)
		{
			OverlappedActors.Emplace(A);
		}
	}
	if (OverlappedActors.Num() == 0) return;
	for (AActor* A : OverlappedActors)
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Cyan, A->GetName());
	}
	*/
}

void AWeapon::NotifyActorBeginOverlap(AActor* OtherActor)
{
	ABaseCharacterV2* OwningCharacter = Cast<ABaseCharacterV2>(Owner);
	// Check if owner is valid
	if (!OwningCharacter) return;
	// Check if owner allows damage dealing
	if (OwningCharacter->CanApplyDamage == false) return;
	// Weapon can't damage owner
	if (OtherActor == OwningCharacter) return;

	FDamageEvent DmgEvent;
	float Mult = OwningCharacter->WeaponDamageMultiplier;
	OtherActor->TakeDamage(WeaponBaseDamage * Mult, DmgEvent, OwningCharacter->GetController(), OwningCharacter);
}


