// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SanityComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSanityDepletedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSanityChangedSignature, float, NewSanity, float, MaxSanity);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SURVIVALSHOOTER_API USanityComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY()
	float Sanity;

	UPROPERTY(EditDefaultsOnly, Category = "Default")
	float MaxSanity;

	// Drain rate in one second
	UPROPERTY()
	float SanityDrainRate = 0.1f;

public:	
	// Sets default values for this component's properties
	USanityComponent();

	UPROPERTY(BlueprintAssignable, Category = "Default")
	FOnSanityChangedSignature OnSanityChanged;

	UPROPERTY(BlueprintAssignable, Category = "Default")
	FOnSanityDepletedSignature OnSanityDepleted;


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	FTimerHandle SanityTimerHandle;

public:
	UFUNCTION(BlueprintCallable)
	float GetSanity() const;

	UFUNCTION(BlueprintCallable)
	float GetMaxSanity() const;

	UFUNCTION(BlueprintCallable)
	void UpdateSanityOnSleep(int SleepTime);

	void DecreaseSanity();

	UFUNCTION(BlueprintCallable)
	void UpdateSanity(float DeltaSanity);
};
