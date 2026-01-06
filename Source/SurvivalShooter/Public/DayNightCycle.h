// Taken and adapted from Mayan Grover blog on Outscal
// https://outscal.com/blog/day-night-cycle-ue5 13.10.2025
// With added be me things for sleep system

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DirectionalLight.h"
#include "DayNightCycle.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimeChangedSignature, float, TimeOfDay);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDayChangedSignature, int32, DayCount);

UCLASS()
class SURVIVALSHOOTER_API ADayNightCycle : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADayNightCycle();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Day/Night Cycle")
	ADirectionalLight* SunLight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Day/Night Cycle")
	ADirectionalLight* MoonLight;
	
	// Current time on a 24-hour clock
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Day/Night Cycle", meta = (ClampMin = "0.0", ClampMax = "24.0"))
	float TimeOfDay;

	// The duration of a full day in minutes
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Day/Night Cycle")
	float DayDurationMinutes = 15.0f;
	
	UFUNCTION(BlueprintCallable)
	void SetTimeOfDay(float NewTimeOfDay);

	UFUNCTION(BlueprintCallable)
	void SetDaysPassed(int32 NewDaysPassed);

	UPROPERTY(BlueprintAssignable, Category = "Day/Night Cycle")
	FOnTimeChangedSignature OnTimeChanged;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Day/Night Cycle")
	int32 DaysPassed = 1;

	UPROPERTY(BlueprintAssignable, Category = "Day/Night Cycle")
	FOnDayChangedSignature OnDayChanged;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Day/Night Cycle")
	int32 DaysToWin;

	UPROPERTY(BlueprintAssignable, Category = "Day/Night Cycle")
	FOnDayChangedSignature OnPlayerWon;

	bool bHasWon = false;

	float WinTime = 5.5f;

	UFUNCTION(BlueprintCallable)
	void SetDaysToWin(int32 NewDaysToWin);

private:
	// Function to update the sun's position based on TimeOfDay
	void UpdateSunPosition();

	FTimerHandle SunUpdateTimerHandle;

	float SunPitch = 0.0f;
	float MoonPitch = 0.0f;

	int32 EasyMinDaysToWin = 4;
	int32 EasyMaxDaysToWin = 6;

	int32 NormalMinDaysToWin = 8;
	int32 NormalMaxDaysToWin = 10;

	int32 HardMinDaysToWin = 13;
	int32 HardMaxDaysToWin = 15;

public:
	FORCEINLINE float GetTimeOfDay() const { return TimeOfDay; };
	FORCEINLINE int32 GetDaysPassed() const { return DaysPassed; };
};
