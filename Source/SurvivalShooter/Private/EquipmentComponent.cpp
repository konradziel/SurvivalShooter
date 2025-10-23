#include "EquipmentComponent.h"

// Sets default values for this component's properties
UEquipmentComponent::UEquipmentComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// Initialization of slots
	EquipmentSlots.SetNum(MaxSlots);
	for (int32 i = 0; i < MaxSlots; i++)
	{
		EquipmentSlots[i] = FEquipmentSlot();
	}
}


// Called when the game starts
void UEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();	
}

bool UEquipmentComponent::AddItem(AItem* ItemToAdd, int32 Quantity)
{
	if (!ItemToAdd || Quantity <= 0)
	{
		return false;
	}

	if (!HasSpaceForItem(ItemToAdd, Quantity))
	{
		return false;
	}

	for (int32 i = 0; i < MaxSlots; i++)
	{
		if (!EquipmentSlots[i].bIsEmpty && EquipmentSlots[i].Item == ItemToAdd && EquipmentSlots[i].Quantity < ItemToAdd->GetItemMaxStackQuantity())
		{
			if ((EquipmentSlots[i].Quantity + Quantity) <= ItemToAdd->GetItemMaxStackQuantity())
			{
				EquipmentSlots[i].Quantity += Quantity;
			}
			else
			{
				int32 QuantityOverLimit = Quantity - (ItemToAdd->GetItemMaxStackQuantity() - EquipmentSlots[i].Quantity);
				EquipmentSlots[i].Quantity += Quantity - QuantityOverLimit;
				if (!HasSpaceForItem(ItemToAdd, QuantityOverLimit))
				{
					// TODO: Implement what to do when we can't take more of this item
				}

			}
		}
	}
}

bool UEquipmentComponent::RemoveItem(int32 SlotIndex, int32 Quantity)
{
	return false;
}

bool UEquipmentComponent::SwapItems(int32 SlotIndex1, int32 SlotIndex2)
{
	return false;
}

bool UEquipmentComponent::IsSlotEmpty(int32 SlotIndex) const
{
	return false;
}

FEquipmentSlot UEquipmentComponent::GetSlot(int32 SlotIndex) const
{
	return false;
}

int32 UEquipmentComponent::GetFirstEmptySlot() const
{
	return false;
}

bool UEquipmentComponent::HasSpaceForItem(AItem* Item, int32 Quantity) const
{
	return false;
}


