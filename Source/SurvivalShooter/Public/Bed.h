// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractWidgetInterface.h"
#include "Bed.generated.h"

class UStaticMeshComponent;
class UBoxComponent;
class UWidgetComponent;
class USphereComponent;

class ADayNightCycle;
class AMainCharacter;

UCLASS()
class SURVIVALSHOOTER_API ABed : public AActor, public IInteractWidgetInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABed();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	void OnSphereEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

public:	
	void SleepInBed(AMainCharacter* MainCharacter);

	virtual void SetWidgetVisibility(bool bIsVisible) override;

	virtual void Interact(AMainCharacter* Character) override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bed Properties", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* BedMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bed Properties", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* CollisionBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bed Properties", meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* SleepWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bed Properties", meta = (AllowPrivateAccess = "true"))
	USphereComponent* SphereForWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bed Properties", meta = (AllowPrivateAccess = "true"))
	ADayNightCycle* DayNightCycle;
};
