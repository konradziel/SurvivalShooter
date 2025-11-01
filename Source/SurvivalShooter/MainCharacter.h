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

	void PickupItem();

    void EquipActiveSlotItem();

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Health, meta = (AllowPrivateAccess = "true"))
	UHealthComponent* HealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Health, meta = (AllowPrivateAccess = "true"))
	USanityComponent* SanityComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Equipment, meta = (AllowPrivateAccess = "true"))
	UEquipmentComponent* EquipmentComponent;


	float DefaultWalkSpeed;
	float DefaultRunSpeed;

	bool IsUnderCrosshair(FHitResult& OutHitResult);

	AItem* LastHitItem = nullptr;
	AItem* HitItem = nullptr;
	AItem* EquippedItem = nullptr;
	
public:
	/** Returns CameraBoom **/
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns Camera */
	FORCEINLINE UCameraComponent* GetCamera() const { return Camera; }

	/** Returns Run status */
	bool GetRunStatus();

};
