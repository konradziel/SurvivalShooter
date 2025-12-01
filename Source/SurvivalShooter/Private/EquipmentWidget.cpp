// Fill out your copyright notice in the Description page of Project Settings.

#include "EquipmentWidget.h"
#include "EquipmentComponent.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Border.h"

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

	Super::UpdateSlotWidget(SlotIndex, SlotData);

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
        TargetEquipmentComponent->OnActiveSlotChanged.RemoveDynamic(this, &UEquipmentWidget::OnActiveSlotChanged);
    }

    Super::SetTargetEquipmentComponent(NewEquipmentComponent);

    if (TargetEquipmentComponent.IsValid())
    {
        TargetEquipmentComponent->OnActiveSlotChanged.AddDynamic(this, &UEquipmentWidget::OnActiveSlotChanged);
        OnActiveSlotChanged(TargetEquipmentComponent->GetActiveSlotIndex());
    }
}