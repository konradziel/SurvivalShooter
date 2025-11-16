// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Magazine.h"
#include "Weapon.generated.h"

/**
 * 
 */
UCLASS()
class SURVIVALSHOOTER_API AWeapon : public AItem
{
	GENERATED_BODY()
	
public:
	AWeapon();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* WeaponSkeletalMesh;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	float FireRate = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	float Damage = 20.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	int32 CurrentAmmoInMagazine = 0;

	void Shoot(class AMainCharacter* Character);

	bool Reload(class AMainCharacter* Character);

private:
	AMagazine* FindMagazineInInventory(class AMainCharacter* Character, int32& SlotIndex);

public:
	FORCEINLINE int32 GetCurrentAmmo() const { return CurrentAmmoInMagazine; }
	FORCEINLINE bool HasAmmo() const { return CurrentAmmoInMagazine > 0; }
};
