// Fill out your copyright notice in the Description page of Project Settings.


#include "Bed.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "../MainCharacter.h"
#include "DayNightCycle.h"

// Sets default values
ABed::ABed()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	UStaticMeshComponent* StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	BedMesh = StaticMesh;
	SetRootComponent(BedMesh);

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(BedMesh);
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(
		ECollisionChannel::ECC_Visibility,
		ECollisionResponse::ECR_Block);

	SleepWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickUpWidget"));
	SleepWidget->SetupAttachment(BedMesh);
	SleepWidget->SetVisibility(false);
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

void ABed::SleepInBed(AMainCharacter* MainCharacter)
{
	if (!MainCharacter)
	{
		return;
	}

	float CurrentTime = DayNightCycle->GetTimeOfDay();
	
	if (CurrentTime >= 5.0f && CurrentTime <= 12.0f)
	{
		DayNightCycle->SetTimeOfDay(CurrentTime + 8.0f);
	}
	else if (CurrentTime > 12.0f)
	{
		DayNightCycle->SetTimeOfDay(20.0f);
	}	
}

