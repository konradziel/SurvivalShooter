#include "Item.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "../MainCharacter.h"
#include "EquipmentComponent.h"

// Sets default values
AItem::AItem()
{

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	UStaticMeshComponent* StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	ItemMesh = StaticMesh;
	SetRootComponent(ItemMesh);

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(ItemMesh);
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(
		ECollisionChannel::ECC_Visibility, 
		ECollisionResponse::ECR_Block);

	PickUpWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickUpWidget"));
	PickUpWidget->SetupAttachment(ItemMesh);
	PickUpWidget->SetVisibility(false);

	SphereForWidget = CreateDefaultSubobject<USphereComponent>(TEXT("SphereForWidget"));
	SphereForWidget->SetupAttachment(ItemMesh);

	BeamMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BeamMesh"));
	BeamMesh->SetupAttachment(ItemMesh);
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();
	
	if (GetWorld() && ItemMesh && CollisionBox)
	{
		SetItemProperties(ItemState);
	}

	SphereForWidget->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
	SphereForWidget->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);

	RestoreBeamRotation();
}

void AItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);
		if (MainCharacter)
		{
			MainCharacter->AddOverlappedInteractable(this);
			MainCharacter->OverlapInteractables();
		}
	}
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);
		if (MainCharacter)
		{
			MainCharacter->RemoveOverlappedInteractable(this);
			MainCharacter->OverlapInteractables();
		}
	}
}

void AItem::RestoreBeamRotation()
{
	if (BeamMesh)
	{
		BeamMesh->SetUsingAbsoluteRotation(true);
		BeamMesh->SetWorldRotation(FRotator::ZeroRotator);
	}
}

bool AItem::CanBePickedUp() const
{
	return ItemState == EItemState::EIS_OnGround;
}

void AItem::PickUpItem()
{
	UE_LOG(LogTemp, Warning, TEXT("PickUpItem() called for item: %s"), *ItemName);

	if (!CanBePickedUp())
	{
		UE_LOG(LogTemp, Warning, TEXT("Item cannot be picked up"));
		return;
	}

	APawn* Player = GetWorld()->GetFirstPlayerController()->GetPawn();
	UE_LOG(LogTemp, Warning, TEXT("Player pawn found: %s"), Player ? *Player->GetName() : TEXT("NULL"));

	if (AMainCharacter* MainCharacter = Cast<AMainCharacter>(Player))
	{
		UEquipmentComponent* EquipmentComponent = MainCharacter->FindComponentByClass<UEquipmentComponent>();
		if (EquipmentComponent && EquipmentComponent->AddItem(this, Quantity).bSuccess)
		{
			UE_LOG(LogTemp, Warning, TEXT("Pickup successful, calling OnPickup()"));
			OnPickup();
		}
	}
}

void AItem::OnPickup()
{
	// Hide
	ItemState = EItemState::EIS_PickedUp;
	SetItemProperties(ItemState);
    // Keep the actor alive so it can be equipped later
}

void AItem::DestroySelf()
{
	Destroy();
}

void AItem::OnEquipped(AMainCharacter* OwnerCharacter)
{
    if (!OwnerCharacter)
    {
        return;
    }

    // Attach the item actor to the character's right hand socket and show it
    USkeletalMeshComponent* CharacterMesh = OwnerCharacter->GetMesh();
    if (!CharacterMesh)
    {
        return;
    }

	FName HandSocketName;
	switch (ItemType)
	{
		case EItemType::EIT_Weapon:
			HandSocketName = TEXT("RightHandWeaponSocket");
			break;
		case EItemType::EIT_Food:
			HandSocketName = TEXT("RightHandFoodSocket");
			break;
		case EItemType::EIT_Magazine:
			HandSocketName = TEXT("RightHandMagazineSocket");
			break;
		default:
			HandSocketName = TEXT("RightHandSocket");
			break;
	}

    AttachToComponent(CharacterMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, HandSocketName);
	ItemState = EItemState::EIS_Equipped;
	SetItemProperties(ItemState);
}

void AItem::OnDropped(const FVector& DropLocation)
{
	SetActorLocation(DropLocation);

	// Update state to OnGround
	ItemState = EItemState::EIS_OnGround;
	SetItemProperties(ItemState);
}

void AItem::SetItemProperties(EItemState State)
{
	if (!ItemMesh || !CollisionBox)
	{
		UE_LOG(LogTemp, Warning, TEXT("SetItemProperties called but components are null"));
		return;
	}

	switch (State)
	{
	case EItemState::EIS_OnGround:

		SetActorHiddenInGame(false);
		SetActorEnableCollision(true);

		ItemMesh->SetSimulatePhysics(true);
		ItemMesh->SetEnableGravity(true);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
		ItemMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

		ItemMesh->SetMassOverrideInKg(NAME_None, 1.0f, true);
		ItemMesh->SetLinearDamping(0.5f);
		ItemMesh->SetAngularDamping(0.5f);

		ItemMesh->SetOverlayMaterial(OverlayMaterial);

		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionResponseToChannel(
			ECollisionChannel::ECC_Visibility,
			ECollisionResponse::ECR_Block);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

		BeamMesh->SetVisibility(true);

		break;

	case EItemState::EIS_PickedUp:

		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);

		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		ItemMesh->SetOverlayMaterial(nullptr);

		BeamMesh->SetVisibility(false);

		break;

	case EItemState::EIS_Equipped:

		SetActorHiddenInGame(false);
		SetActorEnableCollision(false);

		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		ItemMesh->SetOverlayMaterial(nullptr);

		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		BeamMesh->SetVisibility(false);
		BeamMesh->SetUsingAbsoluteRotation(true);

		break;
	}
}

bool AItem::CanBeUsed() const
{
	return false;
}

bool AItem::UseItem(AMainCharacter* MainCharacter)
{
	return false;
}

void AItem::SetWidgetVisibility(bool bIsVisible)
{
	if (PickUpWidget)
	{
		PickUpWidget->SetVisibility(bIsVisible);
	}
}

void AItem::Interact(AMainCharacter* MainCharacter)
{
	PickUpItem();
}

void AItem::RecycleSelf()
{
	OnItemRecycle.Broadcast(this);

	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
}

void AItem::ResetItem()
{
	SetItemProperties(EItemState::EIS_OnGround);
}
