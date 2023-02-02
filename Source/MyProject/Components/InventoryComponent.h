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

	void AddItemToInventory(FItemData NewItem);
	//UFUNCTION(Server, Reliable)
		void RemoveItemFromInventory(FItemData RemovedItem);

	UFUNCTION(BlueprintCallable)
		void UseConsumable(FItemData ConsumableData);

	UFUNCTION(BlueprintCallable)
		void UseWeapon(FItemData WeaponData);

	//void DropItem();
};
