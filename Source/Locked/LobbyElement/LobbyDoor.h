// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Locked/Interface/Interactable.h"
#include "LobbyDoor.generated.h"


UCLASS()
class LOCKED_API ALobbyDoor : public AActor, public IInteractable
{
	GENERATED_BODY()
	DECLARE_DELEGATE_OneParam(FInteractDelegate, APawn*);

public:
	// Sets default values for this actor's properties
	ALobbyDoor();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Component")
		class UBoxComponent* BoxCollider;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Component")
		class UStaticMeshComponent* DoorMesh;

	UPROPERTY(Replicated)
	class ALobbyCharacter* OverlappingCharacter;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void BindInput();

	virtual void Interact(APawn* InteractionInstigator) override {}

	UFUNCTION()
	virtual void OverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
