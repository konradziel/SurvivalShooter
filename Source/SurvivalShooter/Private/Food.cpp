// Fill out your copyright notice in the Description page of Project Settings.


#include "Food.h"
#include "../MainCharacter.h"
#include "HealthComponent.h"

AFood::AFood()
{
	ItemName = TEXT("Food");
	ItemType = EItemType::EIT_Food;
}

bool AFood::CanBeUsed() const
{
	return true;
}

bool AFood::UseItem(AMainCharacter* MainCharacter)
{
	if (!MainCharacter)
	{
		return false;
	}

	UHealthComponent* HealthComp = MainCharacter->FindComponentByClass<UHealthComponent>();
	if (!HealthComp)
	{
		return false;		
	}

	if (HealthComp->GetHealth() >= HealthComp->GetMaxHealth())
	{
		return false;
	}

	HealthComp->UpdateHealth(HealthRestored);

	RecycleSelf();

	return true;
}
