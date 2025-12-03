// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EquipmentWidget.generated.h"

class UTextBlock;
class UImage;
class UBorder;
class UEquipmentComponent;
struct FEquipmentSlot;

USTRUCT(BlueprintType)
struct FSlotWidget
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UTextBlock> QuantityText;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UImage> ItemIcon;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UBorder> Border;

    FSlotWidget()
    {
        QuantityText = nullptr;
        ItemIcon = nullptr;
        Border = nullptr;
    }
};

UCLASS()
class SURVIVALSHOOTER_API UEquipmentWidget : public UUserWidget
{
	GENERATED_BODY()
    TWeakObjectPtr<UEquipmentComponent> TargetEquipmentComponent;

    UFUNCTION()
    void OnEquipmentChanged(int32 SlotIndex);

    UFUNCTION()
    void OnActiveSlotChanged(int32 ActiveSlotIndex);

protected:
    UPROPERTY(BlueprintReadOnly, Category = "Equipment")
    TArray<FSlotWidget> SlotWidgets;

    UPROPERTY(BlueprintReadOnly, Category = "Equipment")
    int32 HighlightedSlotIndex = 0;

    void UpdateSlotWidget(int32 SlotIndex, const FEquipmentSlot& SlotData);

public:
    UFUNCTION(BlueprintCallable)
    void SetTargetEquipmentComponent(UEquipmentComponent* NewEquipmentComponent);

    UFUNCTION(BlueprintCallable, Category = "Equipment")
    void AddSlotReference(UImage* Icon, UTextBlock* Quantity, UBorder* Border);

    // Call this after all slot references are added and component is set
    UFUNCTION(BlueprintCallable, Category = "Equipment")
    void RefreshAllSlots();

    UFUNCTION(BlueprintCallable, Category = "Equipment")
    void AddSlotWidget(UUserWidget* SlotWidget);
};
