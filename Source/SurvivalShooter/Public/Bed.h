// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableActor.h"
#include "Bed.generated.h"

class ADayNightCycle;
class AMainCharacter;

UCLASS()
class SURVIVALSHOOTER_API ABed : public AInteractableActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABed();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	virtual void Interact(AMainCharacter* MainCharacter) override;

	void SleepInBed(AMainCharacter* MainCharacter);

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bed Properties", meta = (AllowPrivateAccess = "true"))
	ADayNightCycle* DayNightCycle;
};
