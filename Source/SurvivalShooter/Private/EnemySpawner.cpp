// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySpawner.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DayNightCycle.h"
#include "AIController.h"
#include "Enemy.h"

// Sets default values
AEnemySpawner::AEnemySpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();
	
	TArray<AActor*> FoundActors;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADayNightCycle::StaticClass(), FoundActors);

	if (FoundActors.Num() > 0)
	{
		DayNightCycle = Cast<ADayNightCycle>(FoundActors[0]);
	}

	if (DayNightCycle)
	{
		DayNightCycle->OnTimeChanged.AddDynamic(this, &AEnemySpawner::OnTimeChanged);
		DayNightCycle->OnDayChanged.AddDynamic(this, &AEnemySpawner::OnDayChanged);

		UpdateDifficulty(DayNightCycle->GetDaysPassed());
	}

	GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &AEnemySpawner::ManageNightSpawning, 2.0f, true);
}

// Called every frame
void AEnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemySpawner::OnTimeChanged(float TimeOfDay)
{
	bool bIsNight = TimeOfDay >= 20.0f || TimeOfDay <= 6.0f;

	if (bIsNight)
	{
		WakeDormantEnemies();
	}
	else 
	{
		for (AEnemy* Enemy : SpawnedEnemies)
		{
			if (Enemy)
			{
				SetEnemyDormant(Enemy, true);
			}
		}
	}
}

void AEnemySpawner::OnDayChanged(int32 NewDayCount)
{
	UpdateDifficulty(NewDayCount);
}

void AEnemySpawner::SpawnEnemy(bool bIsDormant)
{
	FVector SpawnLocation = GetSpawnLocation();
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	if (EnemyClass)
	{
		AEnemy* SpawnedEnemy = GetWorld()->SpawnActor<AEnemy>(EnemyClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
		if (SpawnedEnemy)
		{
			SpawnedEnemies.Add(SpawnedEnemy);
			SetEnemyDormant(SpawnedEnemy, bIsDormant);
		}
	}
}

void AEnemySpawner::ManageNightSpawning()
{
	if (!DayNightCycle || !EnemyClass) return;

	float Time = DayNightCycle->GetTimeOfDay();
	bool bIsNight = (Time >= 20.0f || Time < 6.0f);

	for (int32 i = SpawnedEnemies.Num() - 1; i >= 0; i--)
	{
		if (!SpawnedEnemies[i] || SpawnedEnemies[i]->IsActorBeingDestroyed())
		{
			SpawnedEnemies.RemoveAt(i);
		}
	}

	if (bIsNight)
	{
		if (SpawnedEnemies.Num() < CurrentMaxEnemies)
		{
			SpawnEnemy(false); // Spawn Active
		}
	}
	else
	{
		if (SpawnedEnemies.Num() < CurrentMaxEnemies)
		{
			SpawnEnemy(true); // Spawn Dormant
		}
	}
}

FVector AEnemySpawner::GetSpawnLocation()
{
	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	FVector Center = Player ? Player->GetActorLocation() : GetActorLocation();

	// Random Point on Circle Border
	float RandomAngle = FMath::RandRange(0.0f, 360.0f);
	FVector Direction = FVector(FMath::Cos(FMath::DegreesToRadians(RandomAngle)), FMath::Sin(FMath::DegreesToRadians(RandomAngle)), 0.0f);

	FVector SpawnPos = Center + (Direction * SpawnRadius);

	FHitResult HitResult;
	FVector StartTrace = SpawnPos;
	StartTrace.Z += 1000.0f;
	FVector EndTrace = SpawnPos;
	EndTrace.Z -= 2000.0f;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	if (Player) QueryParams.AddIgnoredActor(Player);

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		StartTrace,
		EndTrace,
		ECC_WorldStatic,
		QueryParams
	);

	if (bHit)
	{
		SpawnPos = HitResult.ImpactPoint;
		SpawnPos.Z += 88.0f; // Capsule half height avoid spawning in ground
	}
	else
	{
		SpawnPos.Z = Center.Z;
	}

	return SpawnPos;
}

void AEnemySpawner::UpdateDifficulty(int32 DayCount)
{
	CurrentMaxEnemies = BaseMaxEnemies + FMath::FloorToInt(DayCount * DifficultyScaling);
}

void AEnemySpawner::WakeDormantEnemies()
{
	for (AEnemy* Enemy : SpawnedEnemies)
	{
		if (Enemy)
		{
			SetEnemyDormant(Enemy, false);
		}
	}
}

void AEnemySpawner::SetEnemyDormant(AEnemy* Enemy, bool bIsDormant)
{
	if (!Enemy) return;

	if (bIsDormant)
	{
		if (AAIController* AI = Cast<AAIController>(Enemy->GetController()))
		{
			AI->StopMovement();
		}

		if (Enemy->GetCharacterMovement())
		{
			Enemy->GetCharacterMovement()->DisableMovement();
		}
	}
	else
	{
		if (Enemy->GetCharacterMovement())
		{
			Enemy->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		}
	}
}
