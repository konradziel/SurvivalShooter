// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryDisplayWidget.h"
#include "EquipmentWidget.generated.h"

UCLASS()
class SURVIVALSHOOTER_API UEquipmentWidget : public UInventoryDisplayWidget
{
	GENERATED_BODY()

    UFUNCTION()
    void OnActiveSlotChanged(int32 ActiveSlotIndex);

protected:
    UPROPERTY(BlueprintReadOnly, Category = "Equipment")
    int32 HighlightedSlotIndex = 0;

    virtual void UpdateSlotWidget(int32 SlotIndex, const FEquipmentSlot& SlotData);

public:
    virtual void SetTargetEquipmentComponent(UEquipmentComponent* NewEquipmentComponent);
};
