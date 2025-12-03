// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bed.generated.h"

UCLASS()
class SURVIVALSHOOTER_API ABed : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABed();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	void SleepInBed(class AMainCharacter* MainCharacter);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bed Properties", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* BedMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bed Properties", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* CollisionBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bed Properties", meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* SleepWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bed Properties", meta = (AllowPrivateAccess = "true"))
	class ADayNightCycle* DayNightCycle;
};
