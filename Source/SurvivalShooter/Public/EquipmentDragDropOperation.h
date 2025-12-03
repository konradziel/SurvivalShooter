// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "EquipmentComponent.h"
#include "EquipmentDragDropOperation.generated.h"

/**
 * 
 */
UCLASS()
class SURVIVALSHOOTER_API UEquipmentDragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite)
	UEquipmentComponent* SourceEquipmentComponent;

	UPROPERTY(BlueprintReadWrite)
	int32 SourceSlotIndex;

	UPROPERTY(BlueprintReadWrite)
	FEquipmentSlot DraggedSlot;	
};
