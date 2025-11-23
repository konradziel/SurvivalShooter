#include "Item.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
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
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();
	
	if (GetWorld() && ItemMesh && CollisionBox)
	{
		SetItemProperties(ItemState);
	}
}

bool AItem::CanBePickedUp() const
{
	// TODO: Add conditions
	return true;
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

		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionResponseToChannel(
			ECollisionChannel::ECC_Visibility,
			ECollisionResponse::ECR_Block);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

		break;

	case EItemState::EIS_PickedUp:

		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);

		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		break;

	case EItemState::EIS_Equipped:

		SetActorHiddenInGame(false);
		SetActorEnableCollision(false);

		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

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
