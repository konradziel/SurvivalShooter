// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractWidgetInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SURVIVALSHOOTER_API IInteractWidgetInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void SetWidgetVisibility(bool bIsVisible) = 0;

	virtual void Interact(class AMainCharacter* Character) = 0;
};
