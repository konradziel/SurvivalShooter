#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

UENUM(BlueprintType)
enum class EItemState : uint8
{
	EIS_OnGround UMETA(DisplayName = "OnGround"),
	EIS_PickedUp UMETA(DisplayName = "PickedUp"),
	EIS_Equipped UMETA(DisplayName = "Equipped"),
	EIS_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EItemType : uint8
{
	EIT_Weapon UMETA(DisplayName = "Weapon"),
	EIT_Food UMETA(DisplayName = "Food"),
	EIT_Magazine UMETA(DisplayName = "Magazine"),
	EIT_None UMETA(DisplayName = "None")
};

UCLASS()
class SURVIVALSHOOTER_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	FString ItemName = TEXT("Unknown");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	UTexture2D* ItemIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	int32 Quantity = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	int32 MaxStackQuantity = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties")
	EItemState ItemState = EItemState::EIS_OnGround;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties")
	EItemType ItemType = EItemType::EIT_None;

	// Pickup functionality
	UFUNCTION(BlueprintCallable)
	bool CanBePickedUp() const;

	UFUNCTION(BlueprintCallable)
	void PickUpItem();

	// Override to customize pickup behavior
	virtual void OnPickup();

    // Called when the item is equipped by a character
    virtual void OnEquipped(class AMainCharacter* OwnerCharacter);

	virtual void OnDropped(const FVector& DropLocation);

	UFUNCTION(BlueprintCallable)
	void DestroySelf();

	virtual void SetItemProperties(EItemState State);

	virtual bool CanBeUsed() const;

	virtual bool UseItem(class AMainCharacter* MainCharacter);


private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	UMeshComponent* ItemMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* CollisionBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* PickUpWidget;

public:
	FORCEINLINE UWidgetComponent* GetPickUpWidget() const { return PickUpWidget; };
	FORCEINLINE int32 GetItemQuantity() const { return Quantity; };
	FORCEINLINE int32 GetItemMaxStackQuantity() const { return MaxStackQuantity; };
	FORCEINLINE UTexture2D* GetItemIcon() const { return ItemIcon; };
	FORCEINLINE UMeshComponent* GetItemMesh() const { return ItemMesh; };
};
