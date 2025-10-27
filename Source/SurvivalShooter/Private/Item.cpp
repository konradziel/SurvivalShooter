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

	ItemMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ItemMesh"));
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
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);

	// Destroy after delay
	FTimerHandle DestroyTimer;
	GetWorldTimerManager().SetTimer(DestroyTimer, this, &AItem::DestroySelf, 0.1f, false);
}

void AItem::DestroySelf()
{
	Destroy();
}

