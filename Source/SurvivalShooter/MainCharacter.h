// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Engine.h"
#include "Public/Item.h"
#include "MainCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

class UHealthComponent;
class USanityComponent;
class UEquipmentComponent;

UCLASS()
class SURVIVALSHOOTER_API AMainCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMainCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Landed(const FHitResult& Hit) override;

	virtual void Jump() override;

	/** MappingContext for player input. */
	UPROPERTY(EditAnywhere, Category = Input)
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* MoveAction;

	void Move(const FInputActionValue& Value);

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* LookAction;

	void Look(const FInputActionValue& Value);

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* RunAction;

	void RunStart(const FInputActionValue& Value);

	void RunStop(const FInputActionValue& Value);

	bool bIsRunning = false;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* PickupAction;

	void PickupInteractItem();

    void EquipActiveSlotItem();

	void UseEquippedItem();

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* DropAction;

	void DropActiveSlotItem();

	UFUNCTION()
    void HandleActiveSlotChanged(int32 ActiveIndex);

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* Slot0Action;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* Slot1Action;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* Slot2Action;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* Slot3Action;

	void SelectEquipmentSlot0();
	void SelectEquipmentSlot1();
	void SelectEquipmentSlot2();
	void SelectEquipmentSlot3();

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* FireAction;

	void StartFire();
	void StopFire();

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* ReloadAction;

	void ReloadWeapon();

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* AimAction;

	void StartAiming();
	void StopAiming();

	bool bIsAiming = false;

	bool bShouldTraceForInteractables = false;

	TArray<AActor*> OverlappedInteractables;

	void TraceForInteractables();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* Camera;

	float CameraDefaultFOV;
	float CameraAimingFOV;
	float CameraCurrentFOV;
	float CameraZoomInterpSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Health, meta = (AllowPrivateAccess = "true"))
	UHealthComponent* HealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Health, meta = (AllowPrivateAccess = "true"))
	USanityComponent* SanityComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Equipment, meta = (AllowPrivateAccess = "true"))
	UEquipmentComponent* EquipmentComponent;

	float DefaultWalkSpeed;
	float DefaultRunSpeed;

	AItem* LastHitItem = nullptr;
	AItem* HitItem = nullptr;
	AItem* EquippedItem = nullptr;

	AActor* HitActor = nullptr;
	AActor* LastHitActor = nullptr;

	FTimerHandle FireTimerHandle;
	void FireWeapon();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	class USoundCue* HitSound;
	
public:
	/** Returns CameraBoom **/
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns Camera */
	FORCEINLINE UCameraComponent* GetCamera() const { return Camera; }

	FORCEINLINE bool GetIsAiming() const { return bIsAiming; }

	/** Returns Run status */
	FORCEINLINE bool GetRunStatus() const { return bIsRunning; }

	bool IsUnderCrosshair(FHitResult& OutHitResult);

	bool IsWeaponEquipped() const;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	
	void OverlapInteractables();

	void AddOverlappedInteractable(AActor* Actor);
	void RemoveOverlappedInteractable(AActor* Actor);
};
