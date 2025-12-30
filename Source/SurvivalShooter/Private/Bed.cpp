// Fill out your copyright notice in the Description page of Project Settings.


#include "Bed.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "../MainCharacter.h"
#include "DayNightCycle.h"
#include "SanityComponent.h"

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

	SphereForWidget = CreateDefaultSubobject<USphereComponent>(TEXT("SphereForWidget"));
	SphereForWidget->SetupAttachment(BedMesh);
}

// Called when the game starts or when spawned
void ABed::BeginPlay()
{
	Super::BeginPlay();

	SphereForWidget->OnComponentBeginOverlap.AddDynamic(this, &ABed::OnSphereOverlap);
	SphereForWidget->OnComponentEndOverlap.AddDynamic(this, &ABed::OnSphereEndOverlap);
}

void ABed::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);
		if (MainCharacter)
		{
			MainCharacter->AddOverlappedInteractable(this);
			MainCharacter->OverlapInteractables();
		}
	}
}

void ABed::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);
		if (MainCharacter)
		{
			MainCharacter->RemoveOverlappedInteractable(this);
			MainCharacter->OverlapInteractables();
		}
	}
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

void ABed::SetWidgetVisibility(bool bIsVisible)
{
	float CurrentTime = DayNightCycle->GetTimeOfDay();

	if (SleepWidget && CurrentTime >= 6.0f && CurrentTime < 20.0f)
	{
		SleepWidget->SetVisibility(bIsVisible);
	}
	else
	{
		SleepWidget->SetVisibility(false);
	}
}

void ABed::Interact(AMainCharacter* Character)
{
	SleepInBed(Character);
}