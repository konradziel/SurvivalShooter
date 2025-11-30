// Fill out your copyright notice in the Description page of Project Settings.
// References that helped with HealthComponent:
// https://dev.epicgames.com/community/learning/tutorials/VpwX/unreal-engine-health-bars-in-unreal-5-part-1-creating-a-health-component-in-c by Valente Game Dev, access date 13.10.2025


#include "HealthComponent.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	Health = MaxHealth;	
}

float UHealthComponent::GetHealth() const
{
	return Health;
}

float UHealthComponent::GetMaxHealth() const
{
	return MaxHealth;
}

void UHealthComponent::UpdateHealth(float DeltaHealth)
{
	Health += DeltaHealth;
	Health = FMath::Clamp(Health, 0.0f, MaxHealth);

	OnHealthChanged.Broadcast(Health, MaxHealth);

	if (Health <= 0.0f)
	{
		OnHealthDepleted.Broadcast();
	}
}
