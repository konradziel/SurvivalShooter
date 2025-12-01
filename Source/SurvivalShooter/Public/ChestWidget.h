// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryDisplayWidget.h"
#include "Chest.h"
#include "ChestWidget.generated.h"

UCLASS()
class SURVIVALSHOOTER_API UChestWidget : public UInventoryDisplayWidget
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chest Properties")
    AChest* OwningChest;
};