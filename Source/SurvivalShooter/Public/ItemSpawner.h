// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemSpawner.generated.h"

class ADayNightCycle;

UCLASS()
class SURVIVALSHOOTER_API AItemSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemSpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	float InnerRadius = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	float OuterRadius = 5000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	TArray<TSubclassOf<AItem>> FoodSpawned;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	TArray<TSubclassOf<AItem>> MagazineSpawned;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	float RespawnDelay = 30.0f;

	ADayNightCycle* DayNightCycle;

private:	
	int32 FoodTargetCount;
	int32 MagazineTargetCount;

	float ItemMultiplier;

	float EasyItemMultiplier = 1.5f;
	float NormalItemMultiplier = 1.0f;
	float HardItemMultiplier = 0.75f;

	void CalculateTargets();
	void SpawnInitialItems();

	void SpawnItems(TArray<TSubclassOf<AItem>>& ItemClasses, int32 Amount);

	FVector GetRandomSpawnLocation();

	UFUNCTION()
	void OnItemRecycle(AItem* ItemInstance);

	UFUNCTION()
	void RespawnRecycledItem(AItem* ItemInstance);

	UFUNCTION()
	void OnDayChanged(int32 NewDayCount);
};
