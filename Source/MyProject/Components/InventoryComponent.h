// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MyProject/Struct/CardStruct.h"
#include "InventoryComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MYPROJECT_API UInventoryComponent : public UActorComponent
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

	UPROPERTY(EditAnywhere)
		int MaxInventory = 3;

	UPROPERTY(BlueprintReadWrite, Replicated)
		bool bInventoryIsFull = false;

	void AddItemToInventory(FItemData NewItem);
	void RemoveItemFromInventory(FItemData RemovedItem);

	UFUNCTION(BlueprintCallable)
		void UseConsumable(FItemData ConsumableData);

	UFUNCTION(BlueprintCallable)
		void UseWeapon(FItemData WeaponData);

	UFUNCTION(BlueprintCallable, Server,Reliable)
		void Server_DropItem(FItemData ItemData);

	UFUNCTION(BlueprintCallable)
		void DropAllItem();
};
