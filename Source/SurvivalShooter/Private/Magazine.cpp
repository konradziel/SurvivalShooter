// Fill out your copyright notice in the Description page of Project Settings.


#include "Magazine.h"

AMagazine::AMagazine()
{
	MaxStackQuantity = 8;
	ItemName = TEXT("Magazine");
	CurrentAmmoCount = MaxCapacity;
}

int32 AMagazine::AddAmmo(int32 Amount)
{
	int32 AvailableSpace = MaxCapacity - CurrentAmmoCount;
	int32 AmountToAdd = FMath::Min(Amount, AvailableSpace);

	CurrentAmmoCount += AmountToAdd;
	
	return AmountToAdd;
}

int32 AMagazine::RemoveAmmo(int32 Amount)
{
	int32 AmountToRemove = FMath::Min(Amount, CurrentAmmoCount);

	CurrentAmmoCount -= AmountToRemove;

	return AmountToRemove;
}
