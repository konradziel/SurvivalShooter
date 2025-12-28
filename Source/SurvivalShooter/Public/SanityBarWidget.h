// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SanityBarWidget.generated.h"

class UProgressBar;
class UTextBlock;
class USanityComponent;

UCLASS()
class SURVIVALSHOOTER_API USanityBarWidget : public UUserWidget
{
	GENERATED_BODY()
	TWeakObjectPtr<USanityComponent> TargetSanityComponent;

	UFUNCTION(BlueprintCallable)
	void OnSanityChanged(float NewSanity, float MaxSanity);

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UProgressBar> SanityProgressBar;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> SanityTextBlock;

public:
	UFUNCTION(BlueprintCallable)
	void SetTargetSanityComponent(USanityComponent* NewSanityComponent);
};

