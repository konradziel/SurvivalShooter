// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySpawner.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DayNightCycle.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
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

	for (int32 i = 0; i < PermanentDormantCount; ++i)
    {
        SpawnEnemy(true, true, i);
    }

	NextSpawnIndex = SpawnedEnemies.Num();
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
	bool bIsMorning = TimeOfDay >= 5.0f && TimeOfDay < 6.0f;

	if (bIsMorning)
	{
		TriggerFlee();
	}
	else if (bIsNight)
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

void AEnemySpawner::SpawnEnemy(bool bIsDormant, bool bIsPermanentDormant, int32 SpawnIndex)
{
	FVector SpawnLocation = GetSpawnLocation(bIsPermanentDormant, SpawnIndex);

	FVector Center = GetActorLocation();
    FVector DirectionToCenter = (Center - SpawnLocation).GetSafeNormal();
    FRotator LookAtCenter = DirectionToCenter.Rotation();
    FRotator SpawnRotation = FRotator(0.0f, LookAtCenter.Yaw, 0.0f);
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	if (EnemyClass)
	{
		AEnemy* SpawnedEnemy = GetWorld()->SpawnActor<AEnemy>(EnemyClass, SpawnLocation, SpawnRotation, SpawnParams);
		if (SpawnedEnemy)
		{
			SpawnedEnemy->SetSpawnLocation(SpawnLocation);

			if (bIsPermanentDormant)
            {
				SpawnedEnemy->Tags.Add(FName("IsPermanentDormant"));
                PermanentDormantEnemies.Add(SpawnedEnemy);
            }
            else
            {
                SpawnedEnemies.Add(SpawnedEnemy);
				SpawnedEnemy->Tags.Add(FName("SpawnedAtRuntime"));
            }

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

	for (int32 i = PermanentDormantEnemies.Num() - 1; i >= 0; i--)
    {
        if (!PermanentDormantEnemies[i] || PermanentDormantEnemies[i]->IsActorBeingDestroyed())
        {
            PermanentDormantEnemies.RemoveAt(i);
        }
    }

	if (bIsNight)
	{
		if (SpawnedEnemies.Num() < CurrentMaxEnemies)
		{
			SpawnEnemy(false, false, NextSpawnIndex); // Spawn Active
			NextSpawnIndex = (NextSpawnIndex + 1) % 72;
		}
	}
	else
	{
		if (SpawnedEnemies.Num() < CurrentMaxEnemies)
		{
			SpawnEnemy(true, false, NextSpawnIndex); // Spawn Dormant
			NextSpawnIndex = (NextSpawnIndex + 1) % 72;
		}
	}
}

FVector AEnemySpawner::GetSpawnLocation(bool bIsPermanentDormant, int32 SpawnIndex)
{
	FVector Center = GetActorLocation();

	// Random Point on Circle Border
	float Angle;

	if (bIsPermanentDormant)
    {
        float AngleStep = 360.0f / FMath::Max(1, PermanentDormantCount);
        Angle = AngleStep * SpawnIndex;
    }
    else
    {
        const int32 TotalSlots = FMath::Max(1, PermanentDormantCount);
        float AngleStep = 360.0f / TotalSlots;
        Angle = AngleStep * SpawnIndex;
		Angle += AngleStep / 2;
    }

	FVector Direction = FVector(FMath::Cos(FMath::DegreesToRadians(Angle)), FMath::Sin(FMath::DegreesToRadians(Angle)), 0.0f);

	float RandomDeviation = FMath::RandRange(-10.0f, 10.0f);
	FVector SpawnPos = Center + (Direction * (SpawnRadius + RandomDeviation));

	FHitResult HitResult;
	FVector StartTrace = SpawnPos;
	StartTrace.Z += 4000.0f;
	FVector EndTrace = SpawnPos;
	EndTrace.Z -= 2000.0f;

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
		if (AAIController* AIController = Cast<AAIController>(Enemy->GetController()))
		{
			AIController->StopMovement();
			
			if (UBlackboardComponent* Blackboard = AIController->GetBlackboardComponent())
    		{
        		Blackboard->SetValueAsBool(TEXT("IsDormant"), bIsDormant);
    		}
		}

		if (Enemy->GetCharacterMovement())
		{
			Enemy->GetCharacterMovement()->DisableMovement();
		}
	}
	else
	{
		if (AAIController* AIController = Cast<AAIController>(Enemy->GetController()))
		{
			if (UBlackboardComponent* Blackboard = AIController->GetBlackboardComponent())
			{
				Blackboard->SetValueAsBool(TEXT("IsDormant"), bIsDormant);
			}
		}

		if (Enemy->GetCharacterMovement())
		{
			Enemy->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		}
	}
}

void AEnemySpawner::TriggerFlee()
{
    for (AEnemy* Enemy : SpawnedEnemies)
    {
        if (Enemy && Enemy->GetController())
        {
            if (AAIController* AIController = Cast<AAIController>(Enemy->GetController()))
            {
                if (UBlackboardComponent* Blackboard = AIController->GetBlackboardComponent())
                {
                    Blackboard->SetValueAsBool(TEXT("IsFleeing"), true);
                    Blackboard->SetValueAsVector(TEXT("FleeLocation"), Enemy->GetSpawnLocation());
                }
            }
			
			FVector SpawnerCenter = GetActorLocation();
			FVector EnemyLocaction = Enemy->GetActorLocation();
			FVector EnemySpawnLocation = Enemy->GetSpawnLocation();
			
			if ((EnemyLocaction - EnemySpawnLocation).Size() < 100.0f)
			{
				FVector ToCenter = SpawnerCenter - EnemyLocaction;
            	ToCenter.Z = 0.0f;
            	if (!ToCenter.IsNearlyZero())
            	{
					FRotator FaceCenter = FRotator(0.0f, ToCenter.Rotation().Yaw, 0.0f);
					Enemy->SetActorRotation(FaceCenter);
				}
				SetEnemyDormant(Enemy, true);
			}
        }
	}
}
