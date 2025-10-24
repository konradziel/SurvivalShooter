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

FPickupResult UEquipmentComponent::AddItem(AItem* ItemToAdd, int32 Quantity)
{
	if (!ItemToAdd || Quantity <= 0)
	{
		return FPickupResult(false, 0);
	}

	if (!HasSpaceForItem(ItemToAdd, Quantity))
	{
		return FPickupResult(false, Quantity);
	}

	// Adding to stack
	for (int32 i = 0; i < MaxSlots; i++)
	{
		if (!EquipmentSlots[i].bIsEmpty && EquipmentSlots[i].Item == ItemToAdd && EquipmentSlots[i].Quantity < ItemToAdd->GetItemMaxStackQuantity())
		{
			if ((EquipmentSlots[i].Quantity + Quantity) <= ItemToAdd->GetItemMaxStackQuantity())
			{
				// Can add all
				EquipmentSlots[i].Quantity += Quantity;
				OnInventoryChanged.Broadcast(i);
				return FPickupResult(true, Quantity);
			}
			else
			{
				// We want to add more than can hold in one stack
				int32 QuantityOverLimit = Quantity - (ItemToAdd->GetItemMaxStackQuantity() - EquipmentSlots[i].Quantity);
				EquipmentSlots[i].Quantity += Quantity - QuantityOverLimit;
				OnInventoryChanged.Broadcast(i);
				if (!HasSpaceForItem(ItemToAdd, QuantityOverLimit))
				{
					// Try to add rest of stack
					AddItem(ItemToAdd, QuantityOverLimit);
				}
				else
				{
					return FPickupResult(true, Quantity - QuantityOverLimit);
				}
			}
		}
	}

	// Adding to empty slot
	int32 EmptySlot = GetFirstEmptySlot();
	if (EmptySlot != -1)
	{
		EquipmentSlots[EmptySlot].Item = ItemToAdd;
		EquipmentSlots[EmptySlot].Quantity = Quantity;
		EquipmentSlots[EmptySlot].bIsEmpty = false;
		OnInventoryChanged.Broadcast(EmptySlot);
		return FPickupResult(true, Quantity);
	}

	return FPickupResult(false, Quantity);
}

bool UEquipmentComponent::RemoveItem(int32 SlotIndex, int32 Quantity)
{
	if (SlotIndex < 0 || SlotIndex >= EquipmentSlots.Num() || EquipmentSlots[SlotIndex].bIsEmpty)
	{
		return false;
	}

	if (Quantity >= EquipmentSlots[SlotIndex].Quantity)
	{
		// Remove entire stack
		EquipmentSlots[SlotIndex].Item = nullptr;
		EquipmentSlots[SlotIndex].Quantity = 0;
		EquipmentSlots[SlotIndex].bIsEmpty = true;
	}
	else
	{
		// Remove partial stack
		EquipmentSlots[SlotIndex].Quantity -= Quantity;
	}

	OnInventoryChanged.Broadcast(SlotIndex);
	return true;
}

bool UEquipmentComponent::SwapItems(int32 SlotIndex1, int32 SlotIndex2)
{
	if (SlotIndex1 != SlotIndex2 ||
		SlotIndex1 < 0 || SlotIndex1 >= EquipmentSlots.Num() ||
		SlotIndex2 < 0 || SlotIndex2 >= EquipmentSlots.Num()
		)
	{
		return false;
	}
	
	FEquipmentSlot TempSlot = EquipmentSlots[SlotIndex1];
	EquipmentSlots[SlotIndex1] = EquipmentSlots[SlotIndex2];
	EquipmentSlots[SlotIndex2] = TempSlot;

	OnInventoryChanged.Broadcast(SlotIndex1);
	OnInventoryChanged.Broadcast(SlotIndex2);
	return true;
}

bool UEquipmentComponent::IsSlotEmpty(int32 SlotIndex) const
{
	if (SlotIndex < 0 || SlotIndex >= EquipmentSlots.Num())
	{
		return true;
	}
	return EquipmentSlots[SlotIndex].bIsEmpty;
}

FEquipmentSlot UEquipmentComponent::GetSlot(int32 SlotIndex) const
{
	if (SlotIndex < 0 || SlotIndex >= EquipmentSlots.Num())
	{
		return FEquipmentSlot();
	}
	return EquipmentSlots[SlotIndex];
}

int32 UEquipmentComponent::GetFirstEmptySlot() const
{
	return false;
}

bool UEquipmentComponent::HasSpaceForItem(AItem* Item, int32 Quantity) const
{
	return false;
}


