// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Locked/Interface/Interactable.h"
#include "LobbyInteractableBase.generated.h"

UCLASS()
class LOCKED_API ALobbyInteractableBase : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALobbyInteractableBase();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Component")
		class UStaticMeshComponent* ActorMesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Component")
		class UBoxComponent* BoxCollider;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void ShowInteractIndicator(bool bState);

	virtual void Interact(APawn* InteractionInstigator) override {};
};
