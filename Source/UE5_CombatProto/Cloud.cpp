// Fill out your copyright notice in the Description page of Project Settings.


#include "Cloud.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/SceneComponent.h"

// Sets default values
ACloud::ACloud()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SceneComponent = CreateDefaultSubobject<USceneComponent>(FName("Root"));
	RootComponent = SceneComponent;
}

// Called when the game starts or when spawned
void ACloud::BeginPlay()
{
	Super::BeginPlay();

	int num = 10;

	for (int i = 0; i < num; i++)
	{
		FVector p = FVector(100.0f, 0.0f, 0.0f);
		float r = 360 / num * i;
		Points.Emplace(FRotator(0.0f, r, 0.0f).RotateVector(p));
	}
	
}

// Called every frame
void ACloud::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Points.Num() == 0) return;
	for (auto p : Points)
	{
		UKismetSystemLibrary::DrawDebugLine(GetWorld(), GetActorLocation(), p + GetActorLocation(), FLinearColor(1.0f, 0.0f, 0.0f), DeltaTime*2, 2.0f);
	}

	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, FString("Hello"));
	UKismetSystemLibrary::DrawDebugArrow(GetWorld(), GetActorLocation(), GetActorLocation() + FVector(0.0f, 0.0f, 100.0f), 5.0f, FLinearColor::Red, 1.0f, 2.0f);
}

