#include "EquipmentComponent.h"

// Sets default values for this component's properties
UEquipmentComponent::UEquipmentComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}


// Called when the game starts
void UEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();

	// Initialization of slots
	// Moved to BeginPlay to allow changing MaxSlots in editor
	EquipmentSlots.SetNum(MaxSlots);
	for (int32 i = 0; i < MaxSlots; i++)
	{
		EquipmentSlots[i] = FEquipmentSlot();
	}
}

FPickupResult UEquipmentComponent::AddItem(AItem* ItemToAdd, int32 Quantity)
{
	// TODO: Redundant we check it in HasSpaceForItem
	// if (!ItemToAdd || Quantity <= 0)
	//	{
	//		return FPickupResult(false, 0);
	//	}

	if (!HasSpaceForItem(ItemToAdd, Quantity))
	{
		return FPickupResult(false, Quantity);
	}

	// Adding to stack
	for (int32 i = 0; i < EquipmentSlots.Num(); i++)
	{
		if (!EquipmentSlots[i].bIsEmpty && EquipmentSlots[i].Item->GetClass() == ItemToAdd->GetClass() && EquipmentSlots[i].Quantity < ItemToAdd->GetItemMaxStackQuantity())
		{
			if ((EquipmentSlots[i].Quantity + Quantity) <= ItemToAdd->GetItemMaxStackQuantity())
			{
				// Can add all
				EquipmentSlots[i].Quantity += Quantity;
				OnEquipmentChanged.Broadcast(i);
				return FPickupResult(true, Quantity);
			}
			else
			{
				// We want to add more than can hold in one stack
				int32 QuantityOverLimit = Quantity - (ItemToAdd->GetItemMaxStackQuantity() - EquipmentSlots[i].Quantity);
				UE_LOG(LogTemp, Warning, TEXT("QuantityOverLimit: %d"), QuantityOverLimit);
				EquipmentSlots[i].Quantity += Quantity - QuantityOverLimit;
				OnEquipmentChanged.Broadcast(i);
				UE_LOG(LogTemp, Warning, TEXT("Check if we have space for rest of stack"));
				if (HasSpaceForItem(ItemToAdd, QuantityOverLimit))
				{
					UE_LOG(LogTemp, Warning, TEXT("We have space. Adding item."));
					AddItem(ItemToAdd, QuantityOverLimit);
					return FPickupResult(true, Quantity);
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("We don't have space."));
					return FPickupResult(true, Quantity - QuantityOverLimit);
				}
			}
		}
	}

	// Adding to empty slot
	UE_LOG(LogTemp, Warning, TEXT("Adding to empty slot."));
	int32 EmptySlot = GetFirstEmptySlot();
	if (EmptySlot != -1)
	{
		EquipmentSlots[EmptySlot].Item = ItemToAdd;
		EquipmentSlots[EmptySlot].Quantity = Quantity;
		EquipmentSlots[EmptySlot].bIsEmpty = false;
		OnEquipmentChanged.Broadcast(EmptySlot);
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

	OnEquipmentChanged.Broadcast(SlotIndex);
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

	OnEquipmentChanged.Broadcast(SlotIndex1);
	OnEquipmentChanged.Broadcast(SlotIndex2);
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
	for (int32 i = 0; i < EquipmentSlots.Num(); i++)
	{
		if (EquipmentSlots[i].bIsEmpty)
		{
			return i;
		}
	}
	return -1;
}

bool UEquipmentComponent::HasSpaceForItem(AItem* Item, int32 Quantity) const
{
	if (!Item || Quantity <= 0)
	{
		return false;
	}

	// Check for stack
	for (const FEquipmentSlot& Slot : EquipmentSlots)
	{
		if (!Slot.bIsEmpty && Slot.Item->GetClass() == Item->GetClass() && Slot.Quantity < Item->GetItemMaxStackQuantity())
		{
			UE_LOG(LogTemp, Warning, TEXT("Item can be stacked"));
			return true;
		}
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Item can't be stacked. Check for empty slots."));
	return GetFirstEmptySlot() != -1;
}

bool UEquipmentComponent::SetActiveSlot(int32 SlotIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("SetActiveSlot called for index %d"), SlotIndex);
	if (SlotIndex < 0 || SlotIndex >= EquipmentSlots.Num())
	{
		ActiveSlotIndex = 0;
		OnActiveSlotChanged.Broadcast(ActiveSlotIndex);
		return true;
	}

	if (ActiveSlotIndex != SlotIndex)
	{
		ActiveSlotIndex = SlotIndex;
		OnActiveSlotChanged.Broadcast(ActiveSlotIndex);
	}
	return true;
}


