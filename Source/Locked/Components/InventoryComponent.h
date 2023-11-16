// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Locked/Struct/CardStruct.h"
#include "InventoryComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LOCKED_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInventoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Deck", Replicated)
		TArray<FItemData> ItemInventory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int MaxInventory = 3;

	void AddItemToInventory(FItemData NewItem);
	UFUNCTION(Server, Reliable, BlueprintCallable)
		void Server_RemoveItemFromInventory(FItemData RemovedItem);

	UFUNCTION(BlueprintCallable)
		void UseConsumable(FItemData ConsumableData);

	UFUNCTION(BlueprintCallable)
		void UseWeapon(FItemData WeaponData);

	UFUNCTION(BlueprintCallable)
		void UsePawConsumable(FItemData ConsumableData);

	UFUNCTION(BlueprintCallable)
		void UsePawWeapon(FItemData WeaponData);

	UFUNCTION(Server, Reliable)
		void UsedUpRifle();

	UFUNCTION(Server, Reliable)
		void UsedUpSpikeArmor();

	UFUNCTION(Server, Reliable, BlueprintCallable)
		void UsedUpMonkeyPaw();

	UFUNCTION(BlueprintCallable)
		bool SearchForUtilityKit(FItemData& UtilityKit);

	UFUNCTION(BlueprintCallable, Server, Reliable)
		void Server_DropItem(FItemData ItemData);

	UFUNCTION(BlueprintCallable, Server, Reliable)
		void DropAllItem();

	UFUNCTION(Server, Reliable)
		void DestroyAllDroppedItem();

	bool HasSpikeArmor();
};
