// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemSpawner.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Item.h"
#include "Food.h"
#include "Magazine.h"
#include "EnemySpawner.h"


// Sets default values
AItemSpawner::AItemSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AItemSpawner::BeginPlay()
{
	Super::BeginPlay();

	CalculateTargets();
	SpawnInitialItems();
}

void AItemSpawner::CalculateTargets()
{
	FoodTargetCount = 10;
	MagazineTargetCount = 20;


	AActor* FoundActor = UGameplayStatics::GetActorOfClass(GetWorld(), AEnemySpawner::StaticClass());
	if (AEnemySpawner* EnemySpawner = Cast<AEnemySpawner>(FoundActor))
	{
		int32 MaxEnemies = EnemySpawner->GetCurrentMaxEnemies();

		FoodTargetCount = FMath::Max(1, MaxEnemies / 2);
		MagazineTargetCount = FMath::Max(1, MaxEnemies / 9);
	}
}

void AItemSpawner::SpawnInitialItems()
{
	TArray<AActor*> ExistingFood;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFood::StaticClass(), ExistingFood);

	// Bind to existing ones so they respawn if picked up
	for (AActor* Actor : ExistingFood)
	{
		if (AFood* F = Cast<AFood>(Actor))
		{
			F->OnItemRecycle.AddDynamic(this, &AItemSpawner::OnItemRecycle);
		}
	}

	int32 FoodToSpawn = FoodTargetCount - ExistingFood.Num();
	if (FoodToSpawn > 0)
	{
		SpawnItems(FoodSpawned, FoodToSpawn);
	}

	TArray<AActor*> ExistingMags;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMagazine::StaticClass(), ExistingMags);

	for (AActor* Actor : ExistingMags)
	{
		if (AMagazine* M = Cast<AMagazine>(Actor))
		{
			M->OnItemRecycle.AddDynamic(this, &AItemSpawner::OnItemRecycle);
		}
	}

	int32 MagsToSpawn = MagazineTargetCount - ExistingMags.Num();
	if (MagsToSpawn > 0)
	{
		SpawnItems(MagazineSpawned, MagsToSpawn);
	}
}

void AItemSpawner::SpawnItems(TArray<TSubclassOf<AItem>>& ItemClasses, int32 Amount)
{
	if (ItemClasses.Num() == 0) return;

	for (int32 i = 0; i < Amount; i++)
	{
		int32 Index = FMath::RandRange(0, ItemClasses.Num() - 1);
		if (ItemClasses[Index])
		{
			FVector Location = GetRandomSpawnLocation();
			FRotator Rotation = FRotator(0.0f, FMath::RandRange(0.0f, 360.0f), 0.0f);

			if (AItem* NewItem = GetWorld()->SpawnActor<AItem>(ItemClasses[Index], Location, Rotation))
			{
				NewItem->OnItemRecycle.AddDynamic(this, &AItemSpawner::OnItemRecycle);
			}
		}
	}
}

FVector AItemSpawner::GetRandomSpawnLocation()
{
	FVector Center = GetActorLocation();

	float Angle = FMath::RandRange(0.0f, 360.0f);
	float Distance = FMath::RandRange(InnerRadius, OuterRadius);

	FVector Offset(
		FMath::Cos(FMath::DegreesToRadians(Angle)) * Distance,
		FMath::Sin(FMath::DegreesToRadians(Angle)) * Distance,
		0.0f
	);

	FVector SpawnPos = Center + Offset;

	FHitResult HitResult;
	FVector StartTrace = SpawnPos + FVector(0.0f, 0.0f, 4000.0f);
	FVector EndTrace = SpawnPos - FVector(0.0f, 0.0f, 2000.0f);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		StartTrace,
		EndTrace,
		ECC_WorldStatic,
		QueryParams
	);

	if (bHit)
	{
		return HitResult.ImpactPoint + FVector(0.0f, 0.0f, 10.0f);
	}

	return SpawnPos;
}

void AItemSpawner::OnItemRecycle(AItem* ItemInstance)
{
	if (!ItemInstance) return;

	FTimerHandle TimerHandle;
	FTimerDelegate TimerDel;

	// Pass the INSTANCE to the timer
	TimerDel.BindUFunction(this, FName("RespawnRecycledItem"), ItemInstance);

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDel, RespawnDelay, false);
}

void AItemSpawner::RespawnRecycledItem(AItem* ItemInstance)
{
	if (!ItemInstance || !IsValid(ItemInstance)) return;

	FVector NewLoc = GetRandomSpawnLocation();
	FRotator NewRot = FRotator(0.0f, FMath::RandRange(0.0f, 360.0f), 0.0f);

	ItemInstance->SetActorLocationAndRotation(NewLoc, NewRot);
	ItemInstance->ResetItem();
}
