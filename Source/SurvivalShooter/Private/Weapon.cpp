// Fill out your copyright notice in the Description page of Project Settings.
// References that helped with Niagara setup:
// https://dev.epicgames.com/community/learning/tutorials/Gx5j/using-niagara-in-c by JohnX451, access date 30.11.2025 
// https://dev.epicgames.com/documentation/en-us/unreal-engine/how-to-create-a-beam-effect-in-niagara-for-unreal-engine official docs, acces date 30.11.2025


#include "Weapon.h"
#include "../MainCharacter.h"
#include "EquipmentComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

AWeapon::AWeapon()
{
	MaxStackQuantity = 1;
	ItemName = TEXT("Weapon");
	CurrentAmmoInMagazine = 30;
	ItemType = EItemType::EIT_Weapon;
}

void AWeapon::Shoot(AMainCharacter* Character)
{
	if (!Character || !HasAmmo())
	{
		return;
	}

	if (FireSound)
	{
		UGameplayStatics::PlaySound2D(this, FireSound);
	}

	CurrentAmmoInMagazine--;

	FHitResult CrosshairHit;
	FVector Target;
	Character->IsUnderCrosshair(CrosshairHit);
	Target = CrosshairHit.Location;

	FVector ShootingSocketLocation = GetShootingSocketLocation();
	FVector ShootingDirection = (Target - ShootingSocketLocation).GetSafeNormal();
	FVector TraceEnd = ShootingSocketLocation + (ShootingDirection * 50000.f);

	if (FireFlash)
	{
		UNiagaraComponent* NiagaraCompFlash = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			FireFlash,
			ShootingSocketLocation,
			ShootingDirection.Rotation()
		);		
	}

	FHitResult WeaponHit;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.AddIgnoredActor(Character);

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		WeaponHit,
		ShootingSocketLocation,
		TraceEnd,
		ECollisionChannel::ECC_Visibility,
		QueryParams
	);

	UE_LOG(LogTemp, Warning, TEXT("Shoot: "), bHit);
	if (bHit)
	{
		if (FireImpact)
		{
			UNiagaraComponent* NiagaraCompImpact = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				GetWorld(),
				FireImpact,
				WeaponHit.Location
			);
		}
	}

	if (FireBeam)
	{
		UNiagaraComponent* NiagaraCompBeam = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			FireBeam,
			ShootingSocketLocation,
			ShootingDirection.Rotation(),
			FVector(1.f),
			false,
			true
		);

		if (NiagaraCompBeam)
		{
			FVector EndPoint = bHit ? WeaponHit.Location : TraceEnd;

			NiagaraCompBeam->SetVariableVec3(FName("BeamEnd"), EndPoint);
			UE_LOG(LogTemp, Warning, TEXT("Setting BeamEnd to: %s"), *EndPoint.ToString());
			NiagaraCompBeam->Activate(true);
		}
	}	
}

FVector AWeapon::GetShootingSocketLocation() const
{
	if (GetItemMesh())
	{
		return GetItemMesh()->GetSocketLocation(TEXT("ShootingSocket"));
	}
	return GetActorLocation();
}

bool AWeapon::Reload(AMainCharacter* Character)
{
	if (!Character)
	{
		return false;
	}

	UEquipmentComponent* EquimpentComponent = Character->FindComponentByClass<UEquipmentComponent>();
	if (!EquimpentComponent)
	{
		return false;
	}

	// Find magazine in inventory
	int32 MagSlotIndex = -1;
	AMagazine* MagazineInInventory = FindMagazineInInventory(Character, MagSlotIndex);

	if (!MagazineInInventory)
	{
		UE_LOG(LogTemp, Warning, TEXT("No magazine found in inventory"));
		return false;
	}

	int32 MagazineCapacity = MagazineInInventory->GetMaxCapacity();
	
	if (CurrentAmmoInMagazine >= MagazineCapacity)
	{
		UE_LOG(LogTemp, Warning, TEXT("Magazine already full"));
		return false;
	}

	int32 AmmoToLoad = MagazineInInventory->GetAmmoCount();

	if (EquimpentComponent->RemoveItem(MagSlotIndex, 1))
	{
		CurrentAmmoInMagazine = AmmoToLoad;
		UE_LOG(LogTemp, Warning, TEXT("Reloaded: %d/%d"), CurrentAmmoInMagazine, MagazineCapacity);
		return true;
	}

	UE_LOG(LogTemp, Warning, TEXT("Can't reload"));
	return false;
}

AMagazine* AWeapon::FindMagazineInInventory(AMainCharacter* Character, int32& SlotIndex)
{
	if (!Character)
	{
		return nullptr;
	}

	UEquipmentComponent* EquipmentComponent = Character->FindComponentByClass<UEquipmentComponent>();
	if (!EquipmentComponent)
	{
		return nullptr;
	}

	TArray<FEquipmentSlot> Slots = EquipmentComponent->GetEquipmentSlots();

	for (int32 i = 0; i < Slots.Num(); i++)
	{
		if (!Slots[i].bIsEmpty && Slots[i].Item)
		{
			AMagazine* Magazine = Cast<AMagazine>(Slots[i].Item);
			if (Magazine && Slots[i].Quantity > 0)
			{
				SlotIndex = i;
				return Magazine;
			}
		}
	}

	SlotIndex = -1;
	return nullptr;
}

