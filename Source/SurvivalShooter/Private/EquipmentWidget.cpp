// Fill out your copyright notice in the Description page of Project Settings.

#include "EquipmentWidget.h"
#include "EquipmentComponent.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Border.h"
#include "Blueprint/WidgetTree.h"

void UEquipmentWidget::OnEquipmentChanged(int32 SlotIndex)
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

void UEquipmentWidget::OnActiveSlotChanged(int32 ActiveSlotIndex)
{
    UE_LOG(LogTemp, Warning, TEXT("OnActiveSlotChanged: %d"), ActiveSlotIndex);
    if (!TargetEquipmentComponent.IsValid())
    {
        return;
    }

    if (ActiveSlotIndex < 0 || ActiveSlotIndex >(TargetEquipmentComponent->GetMaxSlots() - 1))
    {
        HighlightedSlotIndex = 0;
        return;
    }

    int32 PreviousHighlightedSlotIndex = HighlightedSlotIndex;
    HighlightedSlotIndex = ActiveSlotIndex;

    FEquipmentSlot PreviousSlotData = TargetEquipmentComponent->GetSlot(PreviousHighlightedSlotIndex);
    FEquipmentSlot SlotData = TargetEquipmentComponent->GetSlot(HighlightedSlotIndex);
    
    UpdateSlotWidget(PreviousHighlightedSlotIndex, PreviousSlotData);
    UpdateSlotWidget(HighlightedSlotIndex, SlotData);
}

void UEquipmentWidget::UpdateSlotWidget(int32 SlotIndex, const FEquipmentSlot& SlotData)
{
    FSlotWidget& SlotWidget = SlotWidgets[SlotIndex];

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

    if (SlotWidget.Border)
    {
        int32 ActiveSlot = TargetEquipmentComponent.IsValid() ? TargetEquipmentComponent->GetActiveSlotIndex() : 0;
        if (ActiveSlot == SlotIndex)
        {
            // Set border to active color
            SlotWidget.Border->SetBrushColor(FLinearColor::Yellow);
        }
        else
        {
            // Set border to inactive color
            SlotWidget.Border->SetBrushColor(FLinearColor::Black);
        }
    }
}

void UEquipmentWidget::SetTargetEquipmentComponent(UEquipmentComponent* NewEquipmentComponent)
{
    if (TargetEquipmentComponent == NewEquipmentComponent)
    {
        return;
    }

	if (TargetEquipmentComponent.IsValid())
	{
		TargetEquipmentComponent->OnEquipmentChanged.RemoveDynamic(this, &UEquipmentWidget::OnEquipmentChanged);
        TargetEquipmentComponent->OnActiveSlotChanged.RemoveDynamic(this, &UEquipmentWidget::OnActiveSlotChanged);
    }

	TargetEquipmentComponent = NewEquipmentComponent;

	if (TargetEquipmentComponent.IsValid())
	{
		TargetEquipmentComponent->OnEquipmentChanged.AddDynamic(this, &UEquipmentWidget::OnEquipmentChanged);
        TargetEquipmentComponent->OnActiveSlotChanged.AddDynamic(this, &UEquipmentWidget::OnActiveSlotChanged);

		// Initialize all slots with current data
		for (int32 i = 0; i < SlotWidgets.Num(); i++)
		{
			FEquipmentSlot SlotData = TargetEquipmentComponent->GetSlot(i);
			UpdateSlotWidget(i, SlotData);
		}

        OnActiveSlotChanged(TargetEquipmentComponent->GetActiveSlotIndex());
	}
}

void UEquipmentWidget::AddSlotReference(UImage* Icon, UTextBlock* Quantity, UBorder* Border)
{
    FSlotWidget NewSlot;
    NewSlot.ItemIcon = Icon;
    NewSlot.QuantityText = Quantity;
    NewSlot.Border = Border;
    SlotWidgets.Add(NewSlot);
}

void UEquipmentWidget::RefreshAllSlots()
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


void UEquipmentWidget::AddSlotWidget(UUserWidget* SlotWidget)
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
