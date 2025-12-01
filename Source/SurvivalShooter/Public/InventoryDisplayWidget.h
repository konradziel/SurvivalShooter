// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryDisplayWidget.generated.h"

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
class SURVIVALSHOOTER_API UInventoryDisplayWidget : public UUserWidget
{
    GENERATED_BODY()

protected:
    TWeakObjectPtr<UEquipmentComponent> TargetEquipmentComponent;

    UFUNCTION()
    virtual void OnEquipmentChanged(int32 SlotIndex);

    UPROPERTY(BlueprintReadOnly, Category = "Inventory")
    TArray<FSlotWidget> SlotWidgets;

    void UpdateSlotWidget(int32 SlotIndex, const FEquipmentSlot& SlotData);

public:
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    virtual void SetTargetEquipmentComponent(UEquipmentComponent* NewEquipmentComponent);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void AddSlotReference(UImage* Icon, UTextBlock* Quantity, UBorder* Border);

    // Call this after all slot references are added and component is set
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void RefreshAllSlots();

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void AddSlotWidget(UUserWidget* SlotWidget);

    FORCEINLINE UEquipmentComponent* GetTargetEquipmentComponent() const { return TargetEquipmentComponent.Get(); }
};
