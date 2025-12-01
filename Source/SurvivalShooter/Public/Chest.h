// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableActor.h"
#include "Chest.generated.h"

class UEquipmentComponent;
class UChestWidget;
class AMainCharacter;

UCLASS()
class SURVIVALSHOOTER_API AChest : public AInteractableActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChest();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void OpenChest(AMainCharacter* MainCharacter);
	void CloseChest();

public:	
	virtual void Interact(AMainCharacter* MainCharacter) override;

	void ToggleChest(AMainCharacter* MainCharacter);

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Chest Properties", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UChestWidget> ChestWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Equipment, meta = (AllowPrivateAccess = "true"))
	UEquipmentComponent* EquipmentComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Chest Properties", meta = (AllowPrivateAccess = "true"))
	bool bIsOpen = false;

	AMainCharacter* CurrentUser = nullptr;

	UChestWidget* ChestWidget;

public:
	FORCEINLINE bool IsOpen() const { return bIsOpen; }
};
