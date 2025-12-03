// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryDisplayWidget.h"
#include "EquipmentComponent.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Border.h"
#include "Blueprint/WidgetTree.h"

void UInventoryDisplayWidget::OnEquipmentChanged(int32 SlotIndex)
{
    if (!TargetEquipmentComponent.IsValid())
    {
        return;
    }

    if (SlotIndex < 0 || SlotIndex >(TargetEquipmentComponent->GetMaxSlots() - 1))
    {
        return;
    }

    FEquipmentSlot SlotData = TargetEquipmentComponent->GetSlot(SlotIndex);
    UpdateSlotWidget(SlotIndex, SlotData);
}

void UInventoryDisplayWidget::UpdateSlotWidget(int32 SlotIndex, const FEquipmentSlot& SlotData)
{
    FSlotWidget& SlotWidget = SlotWidgets[SlotIndex];

    UUserWidget* WidgetInst = nullptr;
    if (SlotWidget.QuantityText) WidgetInst = Cast<UUserWidget>(SlotWidget.QuantityText->GetOuter()->GetOuter());
    else if (SlotWidget.ItemIcon) WidgetInst = Cast<UUserWidget>(SlotWidget.ItemIcon->GetOuter()->GetOuter());

    if (WidgetInst && TargetEquipmentComponent.IsValid())
    {
        FObjectProperty* CompProp = CastField<FObjectProperty>(WidgetInst->GetClass()->FindPropertyByName(TEXT("OwningComponent")));
        if (CompProp)
        {
            CompProp->SetObjectPropertyValue_InContainer(WidgetInst, TargetEquipmentComponent.Get());
        }

        FIntProperty* IndexProp = CastField<FIntProperty>(WidgetInst->GetClass()->FindPropertyByName(TEXT("SlotIndex")));
        if (IndexProp)
        {
            IndexProp->SetPropertyValue_InContainer(WidgetInst, SlotIndex);
        }
    }

    if (SlotData.bIsEmpty)
    {
        // Hide slot content
        if (SlotWidget.QuantityText)
        {
            SlotWidget.QuantityText->SetText(FText::FromString(TEXT("")));
        }
        if (SlotWidget.ItemIcon)
        {
            SlotWidget.ItemIcon->SetVisibility(ESlateVisibility::Hidden);
        }
    }
    else if (SlotData.Item)
    {
        // Set item icon texture
        if (SlotWidget.ItemIcon && SlotData.Item->GetItemIcon())
        {
            SlotWidget.ItemIcon->SetBrushFromTexture(SlotData.Item->GetItemIcon());
            SlotWidget.ItemIcon->SetVisibility(ESlateVisibility::Visible);
        }

        // Show quantity if more than 1
        if (SlotWidget.QuantityText)
        {
            if (SlotData.Quantity > 1)
            {
                SlotWidget.QuantityText->SetText(FText::FromString(FString::Printf(TEXT("%d"), SlotData.Quantity)));
            }
            else
            {
                SlotWidget.QuantityText->SetText(FText::FromString(TEXT("")));
            }
        }
    }
}

void UInventoryDisplayWidget::SetTargetEquipmentComponent(UEquipmentComponent* NewEquipmentComponent)
{
    if (TargetEquipmentComponent == NewEquipmentComponent)
    {
        return;
    }

    if (TargetEquipmentComponent.IsValid())
    {
        TargetEquipmentComponent->OnEquipmentChanged.RemoveDynamic(this, &UInventoryDisplayWidget::OnEquipmentChanged);
    }

    TargetEquipmentComponent = NewEquipmentComponent;

    if (TargetEquipmentComponent.IsValid())
    {
        TargetEquipmentComponent->OnEquipmentChanged.AddDynamic(this, &UInventoryDisplayWidget::OnEquipmentChanged);

        // Initialize all slots with current data
        RefreshAllSlots();
    }
}

void UInventoryDisplayWidget::AddSlotReference(UImage* Icon, UTextBlock* Quantity, UBorder* Border)
{
    FSlotWidget NewSlot;
    NewSlot.ItemIcon = Icon;
    NewSlot.QuantityText = Quantity;
    NewSlot.Border = Border;
    SlotWidgets.Add(NewSlot);
}

void UInventoryDisplayWidget::RefreshAllSlots()
{
    if (!TargetEquipmentComponent.IsValid())
    {
        return;
    }

    // Update all slots with current equipment data
    for (int32 i = 0; i < SlotWidgets.Num(); i++)
    {
        FEquipmentSlot SlotData = TargetEquipmentComponent->GetSlot(i);
        UpdateSlotWidget(i, SlotData);
    }
}


void UInventoryDisplayWidget::AddSlotWidget(UUserWidget* SlotWidget)
{
    if (!SlotWidget)
    {
        return;
    }

    UImage* ItemIcon = nullptr;
    UTextBlock* QuantityText = nullptr;
    UBorder* Border = nullptr;

    if (SlotWidget->WidgetTree)
    {
        // Find widgets by name
        ItemIcon = Cast<UImage>(SlotWidget->WidgetTree->FindWidget(TEXT("ItemIcon")));
        QuantityText = Cast<UTextBlock>(SlotWidget->WidgetTree->FindWidget(TEXT("QuantityText")));
        Border = Cast<UBorder>(SlotWidget->WidgetTree->FindWidget(TEXT("Border")));
    }

    // Add the slot if both widgets were found
    if (ItemIcon && QuantityText)
    {
        AddSlotReference(ItemIcon, QuantityText, Border);
    }
}