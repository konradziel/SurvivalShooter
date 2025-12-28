// Taken and adapted from Mayan Grover blog on Outscal
// https://outscal.com/blog/day-night-cycle-ue5 by Mayan Grover, access date 13.10.2025
// With added by me things for sleep system and sun and moon control

#include "DayNightCycle.h"
#include "Components/LightComponent.h"
#include "ClockWidget.h"

// Sets default values
ADayNightCycle::ADayNightCycle()
{
 	PrimaryActorTick.bCanEverTick = true;

}

void ADayNightCycle::BeginPlay()
{
	Super::BeginPlay();

	TimeOfDay = 5.0f;	

    UpdateSunPosition();

    GetWorldTimerManager().SetTimer(
        SunUpdateTimerHandle,
        this,
        &ADayNightCycle::UpdateSunPosition,
        1.0f,
        true
    );
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
		DaysPassed++;
		OnDayChanged.Broadcast(DaysPassed);
    }

	OnTimeChanged.Broadcast(TimeOfDay);
    // Update the sun's visual position
    
}

void ADayNightCycle::SetTimeOfDay(float NewTimeOfDay)
{
    TimeOfDay = FMath::Fmod(NewTimeOfDay, 24.0f);
    OnTimeChanged.Broadcast(TimeOfDay);
    UpdateSunPosition();
}

void ADayNightCycle::UpdateSunPosition()
{
    if (SunLight)
    {
        // Convert TimeOfDay (0-24) to a rotation degree (0-360)
        // We subtract 380 degrees to make 6 time a sunrise
        float SunAngle = ((TimeOfDay + 6.0f) / 24.0f) * 360.0f;
        
		// This if statement adjusts the pitch of the sun to create a more realistic arc
        if (SunAngle <= 180)
        {
            SunPitch = SunAngle;
        }
        else if (SunAngle > 180 && SunAngle < 270)
        {
            SunPitch = ((TimeOfDay / 2 + 9.0f) / 24.0f) * 360.0f;
        }
		else if (SunAngle >= 270)
        {
            SunPitch = (((24 - TimeOfDay) / 2 + 9.0f) / 24.0f) * 360.0f;
        }

        FRotator SunRotation = FRotator(SunPitch, SunAngle, 118.0f);
        SunLight->SetActorRotation(SunRotation);

        float SunIntensity = 0.0f;
        const float FadeRange = 20.0f;

        if (SunAngle >= 180.0f && SunAngle < 360.0f)
        {
            float DistFromSunrise = SunAngle - 180.0f;
            float DistFromSunset = 360.0f - SunAngle;

            float FadeAlpha = FMath::Min(DistFromSunrise, DistFromSunset) / FadeRange;
            SunIntensity = FMath::Clamp(FadeAlpha, 0.0f, 1.0f);
        }

        if (ULightComponent* LightComp = SunLight->GetLightComponent())
        {
            LightComp->SetIntensity(SunIntensity * 3.0f);
        }

        if (MoonLight)
        {
            FRotator MoonRotation = FRotator(-26.0f, 246.0f, 0.0f);
            MoonLight->SetActorRotation(MoonRotation);

            float MaxMoonIntensity = 0.1f;
            float MoonIntensity = (1.0f - (SunIntensity * 20.0f)) * MaxMoonIntensity;


            if (ULightComponent* LightComp = MoonLight->GetLightComponent())
            {
                LightComp->SetIntensity(MoonIntensity);
                LightComp->SetLightColor(FLinearColor(0.7f, 0.7f, 1.0f)); // Bluish tint
            }
        }
    }    
}

