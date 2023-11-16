// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MainRoomDoor.generated.h"

UCLASS()
class LOCKED_API AMainRoomDoor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMainRoomDoor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* DoorFrame;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* LeftDoor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* RightDoor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int LockCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UWidgetComponent* RemainingKeyCount;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
