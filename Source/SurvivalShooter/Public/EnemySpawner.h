// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"

class AEnemy;
class ADayNightCycle;
class USphereComponent;

UCLASS()
class SURVIVALSHOOTER_API AEnemySpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemySpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	void OnSphereEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AEnemy> EnemyClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning", meta = (AllowPrivateAccess = "true"))
	float SpawnRadius = 2000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning", meta = (AllowPrivateAccess = "true"))
	int32 PermanentDormantCount = 72;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning", meta = (AllowPrivateAccess = "true"))
	int32 BaseMaxEnemies = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning", meta = (AllowPrivateAccess = "true"))
	float DifficultyScaling = 10.0f;

	ADayNightCycle* DayNightCycle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boundary", meta = (AllowPrivateAccess = "true"))
	USphereComponent* BoundarySphere;

private:
	FTimerHandle SpawnTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning", meta = (AllowPrivateAccess = "true"))
	TArray<AEnemy*> SpawnedEnemies;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning", meta = (AllowPrivateAccess = "true"))
	TArray<AEnemy*> PermanentDormantEnemies; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning", meta = (AllowPrivateAccess = "true"))
	int32 CurrentMaxEnemies;

	int32 NextSpawnIndex = 0;

	UFUNCTION()
	void OnTimeChanged(float TimeOfDay);

	UFUNCTION()
	void OnDayChanged(int32 NewDayCount);

	void SpawnEnemy(bool bIsDormant, bool bIsPermanentDormant = false, int32 SpawnIndex = 0);
	void ManageNightSpawning();

	FVector GetSpawnLocation(bool bIsPermanentDormant = false, int32 SpawnIndex = 0);

	void UpdateDifficulty(int32 DayCount);

	void WakeDormantEnemies();

	void SetEnemyDormant(AEnemy* Enemy, bool bIsDormant);

	void TriggerFlee();

public:
	UFUNCTION(BlueprintCallable, Category = "Spawning")
	int32 GetCurrentMaxEnemies() const { return CurrentMaxEnemies; }
};
