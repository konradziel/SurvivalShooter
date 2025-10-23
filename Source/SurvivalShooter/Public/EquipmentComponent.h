#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Item.h"
#include "EquipmentComponent.generated.h"

USTRUCT(BleprintType)
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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryChanged, int32, SlotIndex);

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
	bool AddItem(AItem* ItemToAdd, int32 Quantity = 1);

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

	UPROPERTY(BlueprintAssignable)
	FOnInventoryChanged OnInventoryChanged;

	FORCEINLINE TArray<FEquipmentSlot> GetEquipmentSlots() const { return EquipmentSlots; }
	FORCEINLINE int32 GetMaxSlots() const { return MaxSlots; }		
};
