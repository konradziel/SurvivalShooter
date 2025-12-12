// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "HealthComponent.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "HealthBarWidget.h"
#include "AI/EnemyAIController.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

// Sets default values
AEnemy::AEnemy()
{
	AttackLeft = TEXT("AttackLeft");
	AttackRight = TEXT("AttackRight");
	AttackBoth = TEXT("AttackBoth");
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));

	AttackRangeSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AttackRangeSphere"));
	AttackRangeSphere->SetupAttachment(RootComponent);

	RightHandBox = CreateDefaultSubobject<UBoxComponent>(TEXT("RightHandBox"));
	RightHandBox->SetupAttachment(GetMesh(), FName("RightHandSocket"));
	LeftHandBox = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftHandBox"));
	LeftHandBox->SetupAttachment(GetMesh(), FName("LeftHandSocket"));
}

void AEnemy::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	if (!HasAuthority())
	{
		return;
	}

	EnemyAIController = Cast<AEnemyAIController>(NewController);

	EnemyAIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
	EnemyAIController->RunBehaviorTree(BehaviorTree);
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(false);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	if (HealthComponent)
	{
		HealthComponent->OnHealthDepleted.AddDynamic(this, &AEnemy::Die);
	}

	GetCharacterMovement()->MaxWalkSpeed = 400.f; 
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	AttackRangeSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::AttackRangeOverlap);
	AttackRangeSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::AttackRangeEndOverlap);

	RightHandBox->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnRightHandOverlap);
	LeftHandBox->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnLeftHandOverlap);

	RightHandBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightHandBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	RightHandBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	RightHandBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	LeftHandBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftHandBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	LeftHandBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	LeftHandBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::BulletHit_Implementation(const FHitResult& HitResult)
{
	if (ImpactParticle)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			ImpactParticle,
			HitResult.Location,
			HitResult.ImpactNormal.Rotation(),
			FVector(1.f),
			true,
			true
		);
	}
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			ImpactSound,
			HitResult.Location
		);
	}
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (HealthComponent && ActualDamage > 0.f)
	{
		HealthComponent->UpdateHealth(-ActualDamage);
	}

	return ActualDamage;
}

void AEnemy::Die()
{
	UE_LOG(LogTemp, Warning, TEXT("Enemy died: %s"), *GetName());
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();

	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));

	GetMesh()->SetSimulatePhysics(true);

	SetLifeSpan(5.0f);
}

void AEnemy::AttackRangeOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	if (OtherActor == nullptr)
	{
		return;
	}

	auto MainCharacter = Cast<ACharacter>(OtherActor);
	if (MainCharacter)
	{
		bInAttackRange = true;
		if (EnemyAIController)
		{
			EnemyAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("InAttackRange"), true);
		}
	}
	
}

void AEnemy::AttackRangeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == nullptr)
	{
		return;
	}

	auto MainCharacter = Cast<ACharacter>(OtherActor);
	if (MainCharacter)
	{
		bInAttackRange = false;
		if (EnemyAIController)
		{
			EnemyAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("InAttackRange"), false);
		}
	}
}

void AEnemy::PlayAttackMontage(FName SectionName, float PlayRate)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AttackMontage)
	{
		AnimInstance->Montage_Play(AttackMontage, PlayRate);
		AnimInstance->Montage_JumpToSection(SectionName, AttackMontage);
	}
}

FName AEnemy::GetAttackSectionName()
{
	FName SectionName;
	const int32 Section{ FMath::RandRange(0, 2) };

	switch (Section)
	{
	case 0:
		SectionName = AttackLeft;
		break;
	case 1:
		SectionName = AttackRight;
		break;
	case 2:
		SectionName = AttackBoth;
		break;
	}

	return SectionName;
}

void AEnemy::DealDamage(AActor* OtherActor)
{
	if (OtherActor == nullptr)
	{
		return;
	}

	auto MainCharacter = Cast<ACharacter>(OtherActor);
	if (MainCharacter)
	{
		UGameplayStatics::ApplyDamage(
			MainCharacter,
			Damage,
			EnemyAIController,
			this,
			UDamageType::StaticClass()
		);
	}
}

void AEnemy::OnRightHandOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Right Hand Overlap with: %s"), *OtherActor->GetName());
	DealDamage(OtherActor);
}

void AEnemy::OnLeftHandOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	DealDamage(OtherActor);
}

void AEnemy::ActivateRightHandCollision()
{
	UE_LOG(LogTemp, Warning, TEXT("Right Hand Collision ACTIVATED"));
	RightHandBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AEnemy::DeactivateRightHandCollision()
{
	UE_LOG(LogTemp, Warning, TEXT("Right Hand Collision DEACTIVATED"));
	RightHandBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::ActivateLeftHandCollision()
{
	LeftHandBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AEnemy::DeactivateLeftHandCollision()
{
	LeftHandBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
