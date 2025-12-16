// Fill out your copyright notice in the Description page of Project Settings.


#include "ClockWidget.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "DayNightCycle.h"

void UClockWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	TArray<AActor*> FoundActors;
	
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADayNightCycle::StaticClass(), FoundActors);

	if (FoundActors.Num() > 0)
	{
		DayNightCycle = Cast<ADayNightCycle>(FoundActors[0]);
	}

	if (DayNightCycle)
	{
		DayNightCycle->OnTimeChanged.AddDynamic(this, &UClockWidget::OnTimeOfDayChanged);
		OnTimeOfDayChanged(DayNightCycle->GetTimeOfDay());

		DayNightCycle->OnDayChanged.AddDynamic(this, &UClockWidget::OnDayChanged);
		OnDayChanged(DayNightCycle->GetDaysPassed());
	}
}

void UClockWidget::OnTimeOfDayChanged(float NewTimeOfDay)
{
	int32 TotalMinutes = static_cast<int32>(NewTimeOfDay * 60.0f);
	int32 Hours = (TotalMinutes / 60) % 24;
	int32 Minutes = TotalMinutes % 60;
	FString TimeString = FString::Printf(TEXT("%02d:%02d"), Hours, Minutes);
	TimeTextBlock->SetText(FText::FromString(TimeString));
}

void UClockWidget::OnDayChanged(int32 Day)
{
	UE_LOG(LogTemp, Warning, TEXT("Day changed to: %d"), Day);
	if (DayTextBlock)
	{
		DayTextBlock->SetText(FText::FromString(FString::Printf(TEXT("Day: %d"), Day)));
	}
}