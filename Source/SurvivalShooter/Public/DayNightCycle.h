// Taken from Mayan Grover blog on Outscal
// https://outscal.com/blog/day-night-cycle-ue5

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DirectionalLight.h"
#include "DayNightCycle.generated.h"

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
	
	// Current time on a 24-hour clock
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Day/Night Cycle", meta = (ClampMin = "0.0", ClampMax = "24.0"))
	float TimeOfDay;

	// The duration of a full day in minutes
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Day/Night Cycle")
	float DayDurationMinutes = 30.0f;

private:
	// Function to update the sun's position based on TimeOfDay
	void UpdateSunPosition();
};
