// Fill out your copyright notice in the Description page of Project Settings.


#include "SanityComponent.h"

// Sets default values for this component's properties
USanityComponent::USanityComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void USanityComponent::BeginPlay()
{
	Super::BeginPlay();
	Sanity = MaxSanity;

	GetWorld()->GetTimerManager().SetTimer(
		SanityTimerHandle,
		this,
		&USanityComponent::DecreaseSanity,
		3600.0f,
		true
	);
}

float USanityComponent::GetSanity() const
{
	return Sanity;
}

float USanityComponent::GetMaxSanity() const
{
	return MaxSanity;
}

void USanityComponent::UpdateSanityOnSleep(int SleepTime)
{
	Sanity = SleepTime * 10;
	Sanity = FMath::Clamp(Sanity, 0.0f, MaxSanity);

	OnSanityChanged.Broadcast(Sanity, MaxSanity);

	if (Sanity <= 0.0f)
	{
		OnSanityDepleted.Broadcast();
	}
}

void USanityComponent::DecreaseSanity()
{
	Sanity = FMath::Clamp(Sanity - SanityDrainRate, 0.0f, MaxSanity);
}


