// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Food.generated.h"

/**
 * 
 */
UCLASS()
class SURVIVALSHOOTER_API AFood : public AItem
{
	GENERATED_BODY()

public:
	AFood();

	virtual bool CanBeUsed() const;
	virtual bool UseItem(class AMainCharacter* MainCharacter);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Food Properties")
	float HealthRestored = 20.0f;
};
