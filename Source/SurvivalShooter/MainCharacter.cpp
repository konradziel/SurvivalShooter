// Fill out your copyright notice in the Description page of Project Settings.
// References that helped with Movement:
// https://dev.epicgames.com/documentation/en-us/unreal-engine/enhanced-input-in-unreal-engine?application_version official docs, access date 11.10.2025



#include "MainCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Public/HealthComponent.h"
#include "Public/SanityComponent.h"
#include "Public/EquipmentComponent.h"
#include "Public/Weapon.h"
#include "Public/InteractableActor.h"
#include "Components/WidgetComponent.h"
#include "Components/SkeletalMeshComponent.h"


// Sets default values
AMainCharacter::AMainCharacter()
{
	DefaultWalkSpeed = 250.f;
	DefaultRunSpeed = 600.f;

	CameraZoomInterpSpeed = 20.f;

	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.5f;
	GetCharacterMovement()->GravityScale = 1.5f;
	GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a camera boom
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 500.0f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->SocketOffset = FVector(0.f, 60.f, 60.f);

	// Create a camera that attaches to Boom
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Camera is attached to the end of Boom
	Camera->bUsePawnControlRotation = false; // Camera wont rotate relatively to Boom

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	SanityComponent = CreateDefaultSubobject<USanityComponent>(TEXT("SanityComponent"));

	EquipmentComponent = CreateDefaultSubobject<UEquipmentComponent>(TEXT("EquipmentComponent"));
}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	UE_LOG(LogTemp, Warning, TEXT("BeginPlay() called."));
	UE_LOG(LogTemp, Warning, TEXT("Character Name: %s"), *GetName());

	if (Camera)
	{
		CameraDefaultFOV = GetCamera()->FieldOfView;
		CameraCurrentFOV = CameraDefaultFOV;
		CameraAimingFOV = 0.8f * CameraDefaultFOV;
	}

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

    // Subscribe to active slot changes to auto-equip
    if (EquipmentComponent)
    {
        EquipmentComponent->OnActiveSlotChanged.AddDynamic(this, &AMainCharacter::HandleActiveSlotChanged);

        HandleActiveSlotChanged(EquipmentComponent->GetActiveSlotIndex());
    }
}


// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FHitResult ActorUnderCrosshair;
	IsUnderCrosshair(ActorUnderCrosshair);
	if (ActorUnderCrosshair.bBlockingHit)
	{
		HitActor = ActorUnderCrosshair.GetActor();

		if ((HitActor != LastHitActor) && LastHitActor)
		{
			if (AItem* LastItem = Cast<AItem>(LastHitActor))
			{
				if (LastItem->GetPickUpWidget())
				{
					LastItem->GetPickUpWidget()->SetVisibility(false);
				}
			}
			else if (AInteractableActor* LastInteractable = Cast<AInteractableActor>(LastHitActor))
			{
				if (LastInteractable->GetInteractWidget())
				{
					LastInteractable->GetInteractWidget()->SetVisibility(false);
				}
			}
		}

		if (AItem* CurrentItem = Cast<AItem>(HitActor))
		{
			if (CurrentItem->GetPickUpWidget())
			{
				CurrentItem->GetPickUpWidget()->SetVisibility(true);
			}
		}
		else if (AInteractableActor* CurrentInteractable = Cast<AInteractableActor>(HitActor))
		{
			if (CurrentInteractable->GetInteractWidget())
			{
				CurrentInteractable->GetInteractWidget()->SetVisibility(true);
			}
		}

		LastHitActor = HitActor;
	}
	else
	{
		HitActor = nullptr;
	}

	if (bIsAiming)
	{
		CameraCurrentFOV = FMath::FInterpTo(
			CameraCurrentFOV,
			CameraAimingFOV,
			DeltaTime,
			CameraZoomInterpSpeed
		);
	}
	else
	{
		CameraCurrentFOV = FMath::FInterpTo(
			CameraCurrentFOV,
			CameraDefaultFOV,
			DeltaTime,
			CameraZoomInterpSpeed
		);
	}
	GetCamera()->SetFieldOfView(CameraCurrentFOV);
}


// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		Input->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMainCharacter::Move);
		
		Input->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		Input->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		Input->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMainCharacter::Look);

		Input->BindAction(RunAction, ETriggerEvent::Started, this, &AMainCharacter::RunStart);
		Input->BindAction(RunAction, ETriggerEvent::Completed, this, &AMainCharacter::RunStop);

		Input->BindAction(PickupAction, ETriggerEvent::Started, this, &AMainCharacter::PickupInteractItem);

		Input->BindAction(DropAction, ETriggerEvent::Started, this, &AMainCharacter::DropActiveSlotItem);

		Input->BindAction(Slot0Action, ETriggerEvent::Started, this, &AMainCharacter::SelectEquipmentSlot0);
		Input->BindAction(Slot1Action, ETriggerEvent::Started, this, &AMainCharacter::SelectEquipmentSlot1);
		Input->BindAction(Slot2Action, ETriggerEvent::Started, this, &AMainCharacter::SelectEquipmentSlot2);
		Input->BindAction(Slot3Action, ETriggerEvent::Started, this, &AMainCharacter::SelectEquipmentSlot3);

		Input->BindAction(FireAction, ETriggerEvent::Started, this, &AMainCharacter::StartFire);
		Input->BindAction(FireAction, ETriggerEvent::Completed, this, &AMainCharacter::StopFire);

		Input->BindAction(ReloadAction, ETriggerEvent::Started, this, &AMainCharacter::ReloadWeapon);

		Input->BindAction(AimAction, ETriggerEvent::Started, this, &AMainCharacter::StartAiming);
		Input->BindAction(AimAction, ETriggerEvent::Completed, this, &AMainCharacter::StopAiming);
	}	
}

void AMainCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	if (bIsRunning)
	{
		GetCharacterMovement()->MaxWalkSpeed = DefaultRunSpeed;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;
	}
}

void AMainCharacter::Jump()
{
	if (!bIsAiming)
	{
		Super::Jump();
	}
}

// Called to move
void AMainCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, LookAxisVector.Y);
		AddMovementInput(RightDirection, LookAxisVector.X);
	}
}

// Called to look
void AMainCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}


// Called to start running
void AMainCharacter::RunStart(const FInputActionValue& Value)
{
	if (Controller != nullptr)
	{
		bIsRunning = true;
		if (!GetCharacterMovement()->IsFalling() && !bIsAiming)
		{
			GetCharacterMovement()->MaxWalkSpeed = DefaultRunSpeed;
		}
	}
}

// Called to stop running
void AMainCharacter::RunStop(const FInputActionValue& Value)
{
	if (Controller != nullptr)
	{
		bIsRunning = false;
		if (!GetCharacterMovement()->IsFalling())
		{
			GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;
		}
	}
}

void AMainCharacter::PickupInteractItem()
{
	UE_LOG(LogTemp, Warning, TEXT("PickupInteractItem() called in MainCharacter"));
	UE_LOG(LogTemp, Warning, TEXT("HitActor is: %s"), HitActor ? *HitActor->GetName() : TEXT("NULL"));
	
	if (AInteractableActor* InteractableActor = Cast<AInteractableActor>(HitActor))
	{
		InteractableActor->Interact(this);
		return;
	}

	if (AItem* Item = Cast<AItem>(HitActor))
	{
		if (Item->CanBePickedUp())
		{
			UE_LOG(LogTemp, Warning, TEXT("Item can be picked up, calling PickUpItem()"));

			Item->PickUpItem();
			EquipActiveSlotItem();
		}
		else if (EquippedItem && EquippedItem->CanBeUsed())
		{
			UseEquippedItem();
		}
	}
}

void AMainCharacter::EquipActiveSlotItem()
{
    if (!EquipmentComponent)
    {
        return;
    }

    const int32 ActiveIndex = EquipmentComponent->GetActiveSlotIndex();
    const FEquipmentSlot Slot = EquipmentComponent->GetSlot(ActiveIndex);

    if (Slot.bIsEmpty || !Slot.Item)
    {
		UE_LOG(LogTemp, Warning, TEXT("EquipActiveSlotItem: Active slot %d is empty"), ActiveIndex);        
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("Equipping item '%s' from slot %d (qty: %d)"), *Slot.Item->ItemName, ActiveIndex, Slot.Quantity);

    Slot.Item->OnEquipped(this);
    EquippedItem = Slot.Item;
}

void AMainCharacter::UseEquippedItem()
{
	if (!EquippedItem || !EquipmentComponent)
	{
		return;
	}

	if (!EquippedItem->CanBeUsed())
	{
		return;
	}

	if (EquippedItem->UseItem(this))
	{
		const int32 ActiveIndex = EquipmentComponent->GetActiveSlotIndex();
		EquipmentComponent->RemoveItem(ActiveIndex, 1);

		if (EquipmentComponent->IsSlotEmpty(ActiveIndex))
		{
			EquippedItem->SetActorHiddenInGame(true);
			EquippedItem->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			EquippedItem = nullptr;
		}
	}	
}

void AMainCharacter::DropActiveSlotItem()
{
	if (!EquipmentComponent)
	{
		return;
	}

	const int32 ActiveIndex = EquipmentComponent->GetActiveSlotIndex();
	const FEquipmentSlot Slot = EquipmentComponent->GetSlot(ActiveIndex);

	if (Slot.bIsEmpty || !Slot.Item)
	{
		UE_LOG(LogTemp, Warning, TEXT("DropActiveSlotItem: Active slot %d is empty"), ActiveIndex);
		return;
	}

	AItem* ItemToDrop = Slot.Item;

	// Unequip
	if (EquippedItem == ItemToDrop)
	{
		EquippedItem->SetActorHiddenInGame(true);
		EquippedItem->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		EquippedItem = nullptr;
	}

	EquipmentComponent->RemoveItem(ActiveIndex, Slot.Quantity);

	FVector CharacterLocation = GetActorLocation();
	FRotator CharacterRotation = GetActorRotation();

	FVector ForwardVector = CharacterRotation.Vector();
	FVector DropLocation = CharacterLocation + (ForwardVector * 50.0f);

	ItemToDrop->OnDropped(DropLocation);
}

void AMainCharacter::HandleActiveSlotChanged(int32 ActiveIndex)
{
	if (EquippedItem)
	{
		EquippedItem->SetActorHiddenInGame(true);
		EquippedItem->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		EquippedItem = nullptr;
	}

    EquipActiveSlotItem();
}

void AMainCharacter::SelectEquipmentSlot0()
{
	if (EquipmentComponent)
	{
		EquipmentComponent->SetActiveSlot(0);
	}
}

void AMainCharacter::SelectEquipmentSlot1()
{
	if (EquipmentComponent)
	{
		EquipmentComponent->SetActiveSlot(1);
	}
}

void AMainCharacter::SelectEquipmentSlot2()
{
	if (EquipmentComponent)
	{
		EquipmentComponent->SetActiveSlot(2);
	}
}

void AMainCharacter::SelectEquipmentSlot3()
{
	if (EquipmentComponent)
	{
		EquipmentComponent->SetActiveSlot(3);
	}
}

void AMainCharacter::StartFire()
{
	if (!EquippedItem)
	{
		return;
	}

	AWeapon* Weapon = Cast<AWeapon>(EquippedItem);
	if (!Weapon)
	{
		return;
	}

	// Can fire if aiming
	if (bIsAiming)
	{
		// Fire immediately
		FireWeapon();

		GetWorld()->GetTimerManager().SetTimer(
			FireTimerHandle,
			this,
			&AMainCharacter::FireWeapon,
			Weapon->FireRate,
			true
		);
	}
}

void AMainCharacter::StopFire()
{
	GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
}

void AMainCharacter::ReloadWeapon()
{
	if (!EquippedItem)
	{
		return;
	}

	AWeapon* Weapon = Cast<AWeapon>(EquippedItem);
	if (!Weapon)
	{
		return;
	}

	Weapon->Reload(this);
}

void AMainCharacter::FireWeapon()
{
	if (!EquippedItem)
	{
		return;
	}

	AWeapon* Weapon = Cast<AWeapon>(EquippedItem);
	if (Weapon)
	{
		Weapon->Shoot(this);
	}
}

void AMainCharacter::StartAiming()
{
	if (!bIsRunning && !GetCharacterMovement()->IsFalling() && IsWeaponEquipped())
	{
		bIsAiming = true;
		bUseControllerRotationYaw = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;
		CameraBoom->TargetArmLength = 250.0f;
		CameraBoom->bUsePawnControlRotation = true;
		CameraBoom->SocketOffset = FVector(0.f, 100.f, 75.f);
	}
}

void AMainCharacter::StopAiming()
{
	bIsAiming = false;
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	CameraBoom->TargetArmLength = 500.0f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->SocketOffset = FVector(0.f, 60.f, 60.f);

	  GetMesh()->UnHideBoneByName(FName("head"));

	if (bIsRunning)
	{
		GetCharacterMovement()->MaxWalkSpeed = DefaultRunSpeed;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;
	}
}

bool AMainCharacter::IsUnderCrosshair(FHitResult& OutHitResult)
{
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;

	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection);

	if (bScreenToWorld)
	{
		const FVector Start{ CrosshairWorldPosition };
		const FVector End{ Start + CrosshairWorldDirection * 50'000.f };
		GetWorld()->LineTraceSingleByChannel(
			OutHitResult,
			Start,
			End,
			ECollisionChannel::ECC_Visibility);

		if (!OutHitResult.bBlockingHit)
		{
			OutHitResult.Location = End;
		}

		if (OutHitResult.bBlockingHit)
		{
			return true;
		}
	}

	return false;
}

bool AMainCharacter::IsWeaponEquipped() const
{
	return EquippedItem && EquippedItem->IsA(AWeapon::StaticClass());
}
