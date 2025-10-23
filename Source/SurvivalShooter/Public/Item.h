#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	int32 Quantity = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	int32 MaxStackQuantity = 1;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* ItemMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* CollisionBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* PickUpWidget;

public:
	FORCEINLINE UWidgetComponent* GetPickUpWidget() const { return PickUpWidget; };
	FORCEINLINE int32 GetItemQuantity() const { return Quantity; };
	FORCEINLINE int32 GetItemMaxStackQuantity() const { return MaxStackQuantity; };
};
