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

		bIsFalling = MainCharacter->GetCharacterMovement()->IsFalling();

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
	}
}

void UMainCHAnimInstance::NativeInitializeAnimation()
{
	MainCharacter = Cast<AMainCharacter>(TryGetPawnOwner());	
}
