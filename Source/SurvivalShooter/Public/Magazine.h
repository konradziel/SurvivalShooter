// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Magazine.generated.h"

/**
 * 
 */
UCLASS()
class SURVIVALSHOOTER_API AMagazine : public AItem
{
	GENERATED_BODY()
	
public:
	AMagazine();

protected:

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magazine Properties", meta = (AllowPrivateAccess = "true"))
	int32 MaxCapacity = 30;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magazine Properties", meta = (AllowPrivateAccess = "true"))
	int32 CurrentAmmoCount = 0;

	UFUNCTION(BlueprintCallable)
	int32 GetAmmoCount() const { return CurrentAmmoCount; }

	UFUNCTION(BlueprintCallable)
	int32 GetMaxCapacity() const { return MaxCapacity; }

	int32 AddAmmo(int32 Amount);

	int32 RemoveAmmo(int32 Amount);

	FORCEINLINE bool IsFull() const { return CurrentAmmoCount >= MaxCapacity; }
	FORCEINLINE bool IsEmpty() const { return CurrentAmmoCount <= 0; }
};
