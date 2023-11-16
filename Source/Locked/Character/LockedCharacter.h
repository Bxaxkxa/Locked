// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "LockedCharacter.generated.h"

UCLASS()
class LOCKED_API ALockedCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ALockedCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		class ARoomTile* CurrentRoom;

	UPROPERTY(Replicated)
		class APlayerControlPawn* ControllerPawn;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void CharacterMoveTo(ARoomTile* Room);

	UFUNCTION(BlueprintCallable)
		void ActiveInput();

	void ChangeToIdleCollision(bool State);

		void Died();

	void Respawn();

	UFUNCTION(NetMulticast, Reliable)
		void RagDoll(bool bState);

	UFUNCTION(Server, Reliable)
		void SetPlayerBackToMainRoom();
};
