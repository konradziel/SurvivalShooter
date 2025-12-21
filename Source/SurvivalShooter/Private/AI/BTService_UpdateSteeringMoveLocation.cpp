// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/BTService_UpdateSteeringMoveLocation.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"
#include "Enemy.h"
#include "NavigationSystem.h"

UBTService_UpdateSteeringMoveLocation::UBTService_UpdateSteeringMoveLocation()
{
	NodeName = TEXT("Update Steering Move Location");
	Interval = 0.15f;
	RandomDeviation = 0.05f;
}

float UBTService_UpdateSteeringMoveLocation::GetOrbitSign(APawn* Pawn)
{
	if (!bRandomizeOrbitDirection || !IsValid(Pawn))
	{
		return 1.0f;
	}

	if (float* Existing = PawnOrbitSign.Find(Pawn))
	{
		return *Existing;
	}

	const float Sign = (FMath::RandBool() ? 1.0f : -1.0f);
	PawnOrbitSign.Add(Pawn, Sign);
	return Sign;
}

void UBTService_UpdateSteeringMoveLocation::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	AAIController* AIController = OwnerComp.GetAIOwner();
	APawn* Pawn = AIController ? AIController->GetPawn() : nullptr;
	AEnemy* SelfEnemy = Pawn ? Cast<AEnemy>(Pawn) : nullptr;
	if (!Blackboard || !AIController || !IsValid(Pawn) || !IsValid(SelfEnemy))
	{
		return;
	}

	UWorld* World = Pawn->GetWorld();
	if (!World)
	{
		return;
	}

	AActor* Target = Cast<AActor>(Blackboard->GetValueAsObject(TargetActorKey.SelectedKeyName));
	if (!IsValid(Target))
	{
		return;
	}

	// Wektory odpowiadające za pozycje własną (przeciwnika) i celu (gracza)
	const FVector SelfLocation = Pawn->GetActorLocation();
	const FVector TargetLocation = Target->GetActorLocation();

	// Różnica między pozycją własną a celu
	// zerujemy współrzędną Z ponieważ operujemy w płaszczyźnie XY,
	// nie chcemy brać pod uwagi wysokości ponieważ nie ma wpływu na prędkość poruszania się
	FVector ToTarget = TargetLocation - SelfLocation;
	ToTarget.Z = 0.0f;
	// Obliczenie długości wektora, żeby otrzymać faktyczną odległość
	const float DistanceToTarget = ToTarget.Size();
	// Normalizujemy wektor, czyli dzielenie przez jego długość, aby otrzymać kierunek
	const FVector SeekDirection = ToTarget.GetSafeNormal();

	// Losowa decyzja czy dany przeciwnik ma krążyć zgodnie czy przeciwnie do ruchu wskazówek zegara
	const float OrbitSign = GetOrbitSign(Pawn);
	// Tworzymy wektor prostopadły do kierunku i mnożymy przez znak decyzji, żeby otrzymać kierunek krążenia wokół celu
	// Mamy z tego styczną do okręgu wokół celu
	const FVector OrbitTangentDirection = FVector(-SeekDirection.Y, SeekDirection.X, 0.0f) * OrbitSign;

	// Mieszanie wag
	// Okresla jak daleko jesteśmy od celu, 0 to blisko, 1 to daleko
	const float Denominator = FMath::Max(1.0f, FarDistance - DesiredRadius);
	const float AlphaFar = FMath::Clamp((DistanceToTarget - DesiredRadius) / Denominator, 0.0f, 1.0f);
	// Lerp - interpolacja liniowa między wagami blisko i daleko od celu.
	// Daleko od celu mamy większą wagę dla dążenia
	// Blisko do celu mamy większą wagę dla krążenia wokół
	const float SeekWeight = FMath::Lerp(SeekWeightNear, SeekWeightFar, AlphaFar);
	const float OrbitWeight = FMath::Lerp(OrbitWeightNear, OrbitWeightFar, AlphaFar);

	// Odpychanie od innych przeciwników, lokalnie
	// Nie ma nadrzędnego managera, który wyznacza pozycje
	FVector Separation = FVector::ZeroVector;
	{
		TArray<FOverlapResult> Overlaps;
		FCollisionQueryParams Params(SCENE_QUERY_STAT(SteeringSeparation), false);
		Params.AddIgnoredActor(Pawn);

		FCollisionObjectQueryParams ObjParams;
		ObjParams.AddObjectTypesToQuery(ECC_Pawn);

		const bool bHit = World->OverlapMultiByObjectType(
			Overlaps,
			SelfLocation,
			FQuat::Identity,
			ObjParams,
			FCollisionShape::MakeSphere(SeparationRadius),
			Params
		);

		if (bHit)
		{
			// Dla każdego przeciwnika, który jest w obszarze
			// Obliczamy wektor od nas do niego i dodajemy go do wektora odpychania
			for (const FOverlapResult& R : Overlaps)
			{
				AEnemy* OtherEnemy = Cast<AEnemy>(R.GetActor());
				if (!IsValid(OtherEnemy) || OtherEnemy == SelfEnemy)
				{
					continue;
				}

				FVector Away = SelfLocation - OtherEnemy->GetActorLocation();
				Away.Z = 0.0f;
				const float D = Away.Size();
				if (D <= KINDA_SMALL_NUMBER)
				{
					continue;
				}

				// Obliczamy siłę odpychania, im bliżej tym większa
				// Używamy funkcji kwadratowej, żeby siła była mniejsza przy większych odległościach
				const float Strength = FMath::Square(1.0f - FMath::Clamp(D / SeparationRadius, 0.0f, 1.0f));
				Separation += Away.GetSafeNormal() * Strength;
			}
		}
	}

	// Łączenie kierunków sił w raz z ich wagami
	FVector Force =
		(SeekDirection * SeekWeight) +
		(OrbitTangentDirection * OrbitWeight) +
		(Separation.GetSafeNormal() * SeparationWeight);

	if (Force.IsNearlyZero())
	{
		Force = SeekDirection;
	}

	Force.Z = 0.0f;
	const FVector DesiredDir = Force.GetSafeNormal();

	FVector Goal = SelfLocation + (DesiredDir * StepDistance);

	// Jeśli jesteśmy "blisko" to idziemy prosto do celu.
	if (DistanceToTarget < (DesiredRadius + (2 * StepDistance)))
	{
		FVector ToGoal = Goal - TargetLocation;
		ToGoal.Z = 0.0f;
		if (!ToGoal.IsNearlyZero())
		{
			Goal = TargetLocation + ToGoal.GetSafeNormal() * DesiredRadius;
		}
	}

	if (UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World))
	{
		FNavLocation Projected;
		if (NavSys->ProjectPointToNavigation(Goal, Projected, FVector(250.0f, 250.0f, 300.0f)))
		{
			Goal = Projected.Location;
		}
	}

	Blackboard->SetValueAsVector(SteeringMoveLocationKey.SelectedKeyName, Goal);

	if (bDrawDebug)
	{
		// Colors
		const FColor SeekColor = FColor::Cyan;
		const FColor TangentColor = FColor::Purple;
		const FColor SepColor = FColor::Orange;
		const FColor ForceColor = FColor::Green;
		const FColor GoalColor = FColor::Yellow;
		const FColor RingColor = FColor(80, 80, 255);

		const float Scale = 250.0f;


		const FVector SeekEnd = SelfLocation + (SeekDirection * Scale);
		const FVector TangentEnd = SelfLocation + (OrbitTangentDirection * Scale);
		const FVector SepEnd = Separation.IsNearlyZero() ? SelfLocation : (SelfLocation + (Separation.GetSafeNormal() * Scale));
		const FVector ForceEnd = SelfLocation + (DesiredDir * Scale);

		// Local draw (selected viewport / standalone)
		DrawDebugCircle(
			World,
			TargetLocation + FVector(0, 0, 10.0f),
			DesiredRadius,
			48,
			RingColor,
			false,
			DebugLifetime,
			0,
			DebugLineThickness,
			FVector(1, 0, 0),
			FVector(0, 1, 0),
			false
		);

		DrawDebugLine(World, SelfLocation, SeekEnd, SeekColor, false, DebugLifetime, 0, DebugLineThickness);
		DrawDebugLine(World, SelfLocation, TangentEnd, TangentColor, false, DebugLifetime, 0, DebugLineThickness);
		if (!SepEnd.Equals(SelfLocation, 1.0f))
		{
			DrawDebugLine(World, SelfLocation, SepEnd, SepColor, false, DebugLifetime, 0, DebugLineThickness);
		}
		DrawDebugLine(World, SelfLocation, ForceEnd, ForceColor, false, DebugLifetime, 0, DebugLineThickness);
		DrawDebugSphere(World, Goal + FVector(0, 0, 20.0f), 18.0f, 12, GoalColor, false, DebugLifetime, 0, DebugLineThickness);
		DrawDebugLine(World, SelfLocation, Goal, GoalColor, false, DebugLifetime, 0, 1.0f);
	}
}


