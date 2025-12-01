// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableActor.generated.h"

class UBoxComponent;
class UWidgetComponent;
class AMainCharacter;

UCLASS()
class SURVIVALSHOOTER_API AInteractableActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AInteractableActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactable Properties", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ObjectMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interactable Properties", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* CollisionBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interactable Properties", meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* InteractWidget;

public:
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	virtual void Interact(AMainCharacter* MainCharacter);

	FORCEINLINE UStaticMeshComponent* GetObjectMesh() const { return ObjectMesh; }
	FORCEINLINE UBoxComponent* GetCollisionBox() const { return CollisionBox; }
	FORCEINLINE UWidgetComponent* GetInteractWidget() const { return InteractWidget; }
};
