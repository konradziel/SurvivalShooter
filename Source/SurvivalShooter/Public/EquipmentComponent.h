#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Item.h"
#include "EquipmentComponent.generated.h"

USTRUCT(BlueprintType)
struct FEquipmentSlot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AItem* Item = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Quantity = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsEmpty = true;

	FEquipmentSlot()
	{
		Item = nullptr;
		Quantity = 0;
		bIsEmpty = true;
	}
};

USTRUCT(BlueprintType)
struct FPickupResult
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bSuccess = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 QuantityAdded = 0;

	FPickupResult()
	{
		bSuccess = false;
		QuantityAdded = 0;
	}

	FPickupResult(bool InSuccess, int32 InQuantityAdded)
	{
		bSuccess = InSuccess;
		QuantityAdded = InQuantityAdded;
	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquipmentChanged, int32, SlotIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActiveSlotChanged, int32, ActiveSlotIndex);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SURVIVALSHOOTER_API UEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEquipmentComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	TArray<FEquipmentSlot> EquipmentSlots;

	// Maximum number of slots
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	int32 MaxSlots = 4;

public:
	UFUNCTION(BlueprintCallable)
	FPickupResult AddItem(AItem* ItemToAdd, int32 Quantity = 1);

	UFUNCTION(BlueprintCallable)
	bool RemoveItem(int32 SlotIndex, int32 Quantity = 1);

	UFUNCTION(BlueprintCallable)
	bool SwapItems(int32 SlotIndex1, int32 SlotIndex2);

	UFUNCTION(BlueprintCallable)
	bool IsSlotEmpty(int32 SlotIndex) const;

	UFUNCTION(BlueprintCallable)
	FEquipmentSlot GetSlot(int32 SlotIndex) const;

	UFUNCTION(BlueprintCallable)
	int32 GetFirstEmptySlot() const;

	UFUNCTION(BlueprintCallable)
	bool HasSpaceForItem(AItem* Item, int32 Quantity = 1) const;

	UFUNCTION(BlueprintCallable)
	bool SetActiveSlot(int32 SlotIndex);

	UPROPERTY(BlueprintAssignable)
	FOnEquipmentChanged OnEquipmentChanged;
	
	UPROPERTY(BlueprintAssignable)
	FOnActiveSlotChanged OnActiveSlotChanged;

	UFUNCTION(BlueprintCallable)
	bool CanDropOnSlot(UEquipmentComponent* SourceComponent, int32 SourceSlotIndex, int32 TargetSlotIndex) const;

	UFUNCTION(BlueprintCallable)
	bool MoveItem(UEquipmentComponent* SourceComponent, int32 SourceSlotIndex, int32 TargetSlotIndex);

private:
	int32 ActiveSlotIndex = 0;

public:
	FORCEINLINE TArray<FEquipmentSlot> GetEquipmentSlots() const { return EquipmentSlots; }
	FORCEINLINE int32 GetMaxSlots() const { return MaxSlots; }
	FORCEINLINE int32 GetActiveSlotIndex() const { return ActiveSlotIndex; }
};
