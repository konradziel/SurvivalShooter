// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ClockWidget.generated.h"

class UTextBlock;
class ADayNightCycle;

UCLASS()
class SURVIVALSHOOTER_API UClockWidget : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnTimeOfDayChanged(float NewTimeOfDay);

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> TimeTextBlock;

	ADayNightCycle* DayNightCycle;
};
