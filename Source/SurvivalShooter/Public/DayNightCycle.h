// Taken and adapted from Mayan Grover blog on Outscal
// https://outscal.com/blog/day-night-cycle-ue5 13.10.2025
// With added be me things for sleep system

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DirectionalLight.h"
#include "DayNightCycle.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimeChangedSignature, float, TimeOfDay);

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
	float DayDurationMinutes = 30.0f;
	
	void SetTimeOfDay(float NewTimeOfDay);

	UPROPERTY(BlueprintAssignable, Category = "Day/Night Cycle")
	FOnTimeChangedSignature OnTimeChanged;

private:
	// Function to update the sun's position based on TimeOfDay
	void UpdateSunPosition();

	float SunPitch = 0.0f;
	float MoonPitch = 0.0f;

public:
	FORCEINLINE float GetTimeOfDay() const { return TimeOfDay; };
};
