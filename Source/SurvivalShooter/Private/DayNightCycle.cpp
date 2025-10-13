// Taken from Mayan Grover blog on Outscal
// https://outscal.com/blog/day-night-cycle-ue5

#include "DayNightCycle.h"

// Sets default values
ADayNightCycle::ADayNightCycle()
{
 	PrimaryActorTick.bCanEverTick = true;

}

void ADayNightCycle::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADayNightCycle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (DayDurationMinutes <= 0.0f) return;

    // Calculate how much TimeOfDay should advance this frame
    // 24 hours in a day, DayDurationMinutes converted to seconds
    float TimeAdvance = (24.0f / (DayDurationMinutes * 60.0f)) * DeltaTime;
    TimeOfDay += TimeAdvance;

    // Loop back to 0 after 24 hours
    if (TimeOfDay >= 24.0f)
    {
        TimeOfDay = 0.0f;
    }

    // Update the sun's visual position
    UpdateSunPosition();
}

void ADayNightCycle::UpdateSunPosition()
{
    if (SunLight)
    {
        // Convert TimeOfDay (0-24) to a rotation degree (0-360)
        // We subtract 180 degrees to make 0 time a sunrise
        float SunAngle = (TimeOfDay / 24.0f) * 360.0f;
        FRotator NewRotation = FRotator(SunAngle - 180.0f, 112.0f, 118.0f);
        SunLight->SetActorRotation(NewRotation);
    }
}

