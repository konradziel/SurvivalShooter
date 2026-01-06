// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MyGameInstance.generated.h"

UENUM(BlueprintType)
enum class EGameDifficulty : uint8
{
    EGD_Easy UMETA(DisplayName = "Easy"),
    EGD_Normal UMETA(DisplayName = "Normal"),
    EGD_Hard UMETA(DisplayName = "Hard")
};
/**
 * 
 */
UCLASS()
class SURVIVALSHOOTER_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Difficulty")
	EGameDifficulty GameDifficulty;
};
