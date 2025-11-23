// Taken from Mayan Grover blog on Outscal
// https://outscal.com/blog/day-night-cycle-ue5 13.10.2025
// With added be me things for sleep system

#include "DayNightCycle.h"
#include "Components/LightComponent.h"

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

void ADayNightCycle::SetTimeOfDay(float NewTimeOfDay)
{
    TimeOfDay = FMath::Fmod(NewTimeOfDay, 24.0f);
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

        float SunIntensity = 0.0f;
        if (TimeOfDay >= 6.0f && TimeOfDay < 18.0f)
        {
            SunIntensity = 1.0f;
        }

        if (ULightComponent* LightComp = SunLight->GetLightComponent())
        {
            LightComp->SetIntensity(SunIntensity * 10.0f);
        }
    }

    if (MoonLight)
    {
        float MoonAngle = ((TimeOfDay + 12.0f) / 24.0f) * 360.0f;
        FRotator MoonRotation = FRotator(FMath::Fmod(MoonAngle - 180.0f, 360.0f), 112.0f, 118.0f);
        MoonLight->SetActorRotation(MoonRotation);

        float MoonIntensity = 0.0f;
        if (TimeOfDay < 6.0f || TimeOfDay >= 18.0f)
        {
            MoonIntensity = 0.1f; // 30% of sun brightness
        }

        if (ULightComponent* LightComp = MoonLight->GetLightComponent())
        {
            LightComp->SetIntensity(MoonIntensity * 10.0f);
            LightComp->SetLightColor(FLinearColor(0.7f, 0.7f, 1.0f)); // Bluish tint
        }
    }
}

