// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TargetingComponent.generated.h"

class USoundCue;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE5_COMBATPROTO_API UTargetingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTargetingComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	APawn* Owner;

	void LookAtTarget(float DeltaTime);
	void RotateOwnerToTarget(float DeltaTime);
	bool ActorInView(AActor* Actor);
	bool ActorOccluded(AActor* Actor);
	bool ActorInRangeOfLocation(AActor* Actor, FVector Location, float Range);
	void UpdateTargetingBiasLocation(float RayLength = 10000.0f);
	void SortActorsByDistanceToLocation(TArray<AActor*>& Actors, FVector Location);

	bool IsTargeting = false;
	bool IsDead = false;
	float TargetingRange = 2500.0f;
	FVector TargetingBiasLocation;

	// list of all possible targets in the level
	TArray<AActor*> AllTargets;
	// list of all visible targets in the level
	TArray<AActor*> AllVisibleTargets;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		TSubclassOf<AActor> TargetedActorClass;

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
		AActor* Target = nullptr;
	UPROPERTY(BlueprintReadOnly, Category = "Combat")
		AActor* SuggestedTarget = nullptr;

	bool CanRotateOwner = true;

	void GetNewTarget();
	void ClearTarget();

	UPROPERTY(EditAnywhere)
		USoundCue* TargetingSound;

};
