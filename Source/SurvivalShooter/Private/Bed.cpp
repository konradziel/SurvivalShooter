// Fill out your copyright notice in the Description page of Project Settings.


#include "Bed.h"
#include "../MainCharacter.h"
#include "DayNightCycle.h"
#include "SanityComponent.h"

// Sets default values
ABed::ABed()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void ABed::BeginPlay()
{
	Super::BeginPlay();
	
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADayNightCycle::StaticClass(), FoundActors);

	if (FoundActors.Num() > 0)
	{
		DayNightCycle = Cast<ADayNightCycle>(FoundActors[0]);
	}
}

void ABed::Interact(AMainCharacter* MainCharacter)
{
	SleepInBed(MainCharacter);
}

void ABed::SleepInBed(AMainCharacter* MainCharacter)
{
	if (!MainCharacter)
	{
		return;
	}

	USanityComponent* SanityComp = MainCharacter->FindComponentByClass<USanityComponent>();
	
	if (!SanityComp)
	{
		return;
	}

	float CurrentTime = DayNightCycle->GetTimeOfDay();
	
	if (CurrentTime > 14.0f && CurrentTime < 20.0f)
	{
		SanityComp->UpdateSanityOnSleep(20.0f - CurrentTime);
		DayNightCycle->SetTimeOfDay(20.0f);		
	}
	else if (CurrentTime >= 6.0f && CurrentTime <= 14.0f)
	{
		SanityComp->UpdateSanityOnSleep(7.0f);
		DayNightCycle->SetTimeOfDay(CurrentTime + 7.0f);
	}	
	
}

