// Fill out your copyright notice in the Description page of Project Settings.


#include "SanityBarWidget.h"
#include "SanityComponent.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void USanityBarWidget::OnSanityChanged(float NewSanity, float MaxSanity)
{
	float Percentage = NewSanity / MaxSanity;
	SanityProgressBar->SetPercent(Percentage);
	SanityTextBlock->SetText(FText::FromString(FString::Printf(TEXT("%d%%"), static_cast<int32>(Percentage * 100))));
}

void USanityBarWidget::SetTargetSanityComponent(USanityComponent* NewSanityComponent)
{
	// Early exit if already set to the same component
	if (TargetSanityComponent == NewSanityComponent)
	{
		return;
	}

	//Unregister from the old target Sanity component
	if (TargetSanityComponent.IsValid())
	{
		TargetSanityComponent->OnSanityChanged.RemoveDynamic(this, &USanityBarWidget::OnSanityChanged);
	}

	TargetSanityComponent = NewSanityComponent;

	//Register to the new target Sanity component
	if (TargetSanityComponent.IsValid())
	{
		TargetSanityComponent->OnSanityChanged.AddDynamic(this, &USanityBarWidget::OnSanityChanged);
	}
}