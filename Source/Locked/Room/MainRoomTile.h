// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RoomTile.h"
#include "MainRoomTile.generated.h"

/**
 *
 */
UCLASS()
class LOCKED_API AMainRoomTile : public ARoomTile
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMainRoomTile();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UChildActorComponent* LockedDoorActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Replicated)
		int32 NeededKey = 3;

protected:
	virtual void BeginPlay() override;

public:
	virtual void SpawnDoor() override;

	virtual bool RoomEvent(class APlayerControlPawn* Player) override;

	UFUNCTION(BlueprintCallable)
		void AddKey();
};
