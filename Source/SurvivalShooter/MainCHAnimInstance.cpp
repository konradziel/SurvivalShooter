// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCHAnimInstance.h"
#include "MainCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UMainCHAnimInstance::UpdateAnimProperties(float DeltaTime)
{
	if (MainCharacter == nullptr)
	{
		MainCharacter = Cast<AMainCharacter>(TryGetPawnOwner());
	}
	if (MainCharacter)
	{
		FVector Velocity(MainCharacter->GetVelocity());
		Velocity.Z = 0;

		Speed = Velocity.Size();

		Direction = CalculateDirection(Velocity, MainCharacter->GetActorRotation());

		bIsFalling = MainCharacter->GetCharacterMovement()->IsFalling();

		float VerticalVelocity = MainCharacter->GetVelocity().Z;
		bIsJumping = bIsFalling && VerticalVelocity > 0.f;

		if (MainCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f)
		{
			bIsAccelareting = true;
		}
		else
		{
			bIsAccelareting = false;
		}
		
		bIsRunning = MainCharacter->GetRunStatus();

		bIsWeaponEquipped = MainCharacter->IsWeaponEquipped();

		bIsAiming = MainCharacter->GetIsAiming();

		Pitch = MainCharacter->GetBaseAimRotation().Pitch;
	}
}

void UMainCHAnimInstance::NativeInitializeAnimation()
{
	MainCharacter = Cast<AMainCharacter>(TryGetPawnOwner());	
}
