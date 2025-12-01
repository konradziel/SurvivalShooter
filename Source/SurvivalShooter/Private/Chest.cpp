// Fill out your copyright notice in the Description page of Project Settings.


#include "Chest.h"
#include "EquipmentComponent.h"
#include "ChestWidget.h"
#include "Blueprint/UserWidget.h"
#include "../MainCharacter.h"

// Sets default values
AChest::AChest()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	EquipmentComponent = CreateDefaultSubobject<UEquipmentComponent>(TEXT("EquipmentComponent"));
}

// Called when the game starts or when spawned
void AChest::BeginPlay()
{
	Super::BeginPlay();
}

void AChest::Interact(AMainCharacter* MainCharacter)
{
	ToggleChest(MainCharacter);
}

void AChest::OpenChest(AMainCharacter* MainCharacter)
{
	if (!MainCharacter)
	{
		return;
	}

	if (bIsOpen)
	{
		return;
	}

	bIsOpen = true;
	CurrentUser = MainCharacter;

	APlayerController* PlayerController = Cast<APlayerController>(MainCharacter->GetController());
	if (PlayerController)
	{
		// Create the widget if it doesn't exist
		if (!ChestWidget && ChestWidgetClass)
		{
			ChestWidget = CreateWidget<UChestWidget>(PlayerController, ChestWidgetClass);
			if (ChestWidget)
			{
				ChestWidget->OwningChest = this;
				// Connect the widget to this chest's inventory
				ChestWidget->SetTargetEquipmentComponent(EquipmentComponent);
			}
		}

		// Show the widget
		if (ChestWidget)
		{
			ChestWidget->AddToViewport();
		}

		PlayerController->bShowMouseCursor = true;
		PlayerController->SetInputMode(FInputModeGameAndUI());
	}
}

void AChest::CloseChest()
{
	if (!bIsOpen)
	{
		return;
	}

	bIsOpen = false;

	if (ChestWidget)
	{
		ChestWidget->RemoveFromParent();
	}

	if (CurrentUser)
	{
		APlayerController* PlayerController = Cast<APlayerController>(CurrentUser->GetController());
		if (PlayerController)
		{
			PlayerController->bShowMouseCursor = false;
			PlayerController->SetInputMode(FInputModeGameOnly());
		}
	}

	CurrentUser = nullptr;
}

void AChest::ToggleChest(AMainCharacter* MainCharacter)
{
	if (bIsOpen)
	{
		CloseChest();
	}
	else
	{
		OpenChest(MainCharacter);
	}
}
