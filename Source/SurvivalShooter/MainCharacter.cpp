// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Public/HealthComponent.h"
#include "Public/SanityComponent.h"
#include "Public/EquipmentComponent.h"
#include "Components/WidgetComponent.h"


// Sets default values
AMainCharacter::AMainCharacter()
{
	DefaultWalkSpeed = 350.f;
	DefaultRunSpeed = 700.f;

	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
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
	CameraBoom->TargetArmLength = 300.0f;
	CameraBoom->bUsePawnControlRotation = true;

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

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}


// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FHitResult ItemUnderCrosshair;
	IsUnderCrosshair(ItemUnderCrosshair);
	if (ItemUnderCrosshair.bBlockingHit)
	{
		HitItem = Cast<AItem>(ItemUnderCrosshair.GetActor());

		if ((HitItem != LastHitItem) && (LastHitItem && LastHitItem->GetPickUpWidget())) 
		{
			LastHitItem->GetPickUpWidget()->SetVisibility(false);
		}

		if (HitItem && HitItem->GetPickUpWidget())
		{
			HitItem->GetPickUpWidget()->SetVisibility(true);
			LastHitItem = HitItem;
		}
	}
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

		Input->BindAction(PickupAction, ETriggerEvent::Started, this, &AMainCharacter::PickupItem);
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
		GetCharacterMovement()->MaxWalkSpeed = DefaultRunSpeed;
	}
}

// Called to stop running
void AMainCharacter::RunStop(const FInputActionValue& Value)
{
	if (Controller != nullptr)
	{
		bIsRunning = false;
		GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;
	}
}

bool AMainCharacter::GetRunStatus()
{
	return bIsRunning;
}

void AMainCharacter::PickupItem()
{
	UE_LOG(LogTemp, Warning, TEXT("PickupItem() called in MainCharacter"));
	UE_LOG(LogTemp, Warning, TEXT("HitItem is: %s"), HitItem ? *HitItem->GetName() : TEXT("NULL"));

	if (HitItem && HitItem->CanBePickedUp())
	{
		UE_LOG(LogTemp, Warning, TEXT("Item can be picked up, calling PickUpItem()"));

		HitItem->PickUpItem();
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
		if (OutHitResult.bBlockingHit)
		{
			return true;
		}
	}

	return false;
}
