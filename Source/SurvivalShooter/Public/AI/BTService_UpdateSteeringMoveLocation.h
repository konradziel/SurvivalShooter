// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "BTService_UpdateSteeringMoveLocation.generated.h"

/**
 *
 */
UCLASS()
class SURVIVALSHOOTER_API UBTService_UpdateSteeringMoveLocation : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_UpdateSteeringMoveLocation();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector SteeringMoveLocationKey;

	// Desired ring radius around the target
	UPROPERTY(EditAnywhere, Category = "Steering", meta = (ClampMin = "50.0"))
	float DesiredRadius = 120.0f;

	// Above this distance, Seek dominates and Orbit is reduced to 0
	UPROPERTY(EditAnywhere, Category = "Steering", meta = (ClampMin = "1000.0"))
	float FarDistance = 2000.0f;

	// How far ahead is the temporary MoveTo
	UPROPERTY(EditAnywhere, Category = "Steering", meta = (ClampMin = "50.0"))
	float StepDistance = 200.0f;

	UPROPERTY(EditAnywhere, Category = "Forces", meta = (ClampMin = "0.0"))
	float SeekWeightNear = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Forces", meta = (ClampMin = "0.0"))
	float SeekWeightFar = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Forces", meta = (ClampMin = "0.0"))
	float OrbitWeightNear = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Forces", meta = (ClampMin = "0.0"))
	float OrbitWeightFar = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Forces", meta = (ClampMin = "50.0"))
	float SeparationRadius = 220.0f;

	UPROPERTY(EditAnywhere, Category = "Forces", meta = (ClampMin = "0.0"))
	float SeparationWeight = 1.5f;

	// True - flips orbit direction once per enemy
	UPROPERTY(EditAnywhere, Category = "Steering")
	bool bRandomizeOrbitDirection = true;

	// Debug draw
	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bDrawDebug = true;

	UPROPERTY(EditAnywhere, Category = "Debug", meta = (ClampMin = "0.0"))
	float DebugLifetime = 0.12f;

	UPROPERTY(EditAnywhere, Category = "Debug", meta = (ClampMin = "0.0"))
	float DebugLineThickness = 2.0f;

private:
	TMap<TWeakObjectPtr<APawn>, float> PawnOrbitSign;

	float GetOrbitSign(APawn* Pawn);
};


