// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"
#include "MyProject/Character/PlayerControlPawn.h"
#include "MyProject/Character/BoardController.h"
#include "MyProject/Character/State/LockedPlayerState.h"
#include "Myproject/LockedBFL.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
}


// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

}


// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green, "Inventory Count: " + FString::FromInt(ItemInventory.Num()));
	for (FItemData it : ItemInventory)
	{
		FString EnumString = UEnum::GetValueAsString(it.ItemName);

		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green, "Item Name: " + ULockedBFL::GetEnumName(EnumString));
	}
}

void UInventoryComponent::AddItemToInventory(FItemData NewItem)
{
	ItemInventory.Add(NewItem);

	bInventoryIsFull = ItemInventory.Num() > MaxInventory;
}

void UInventoryComponent::RemoveItemFromInventory(FItemData RemovedItem)
{
	ItemInventory.Remove(RemovedItem);
}

void UInventoryComponent::UseConsumable(FItemData ConsumableData)
{
	switch (ConsumableData.ItemName) {
	case EItemList::EnergyDrink:
		GetOwner<ABoardController>()->GetPlayerState<ALockedPlayerState>()->DoubleMovePoint();
		break;
	default:
		break;
	}

	ItemInventory.Remove(ConsumableData);
}

void UInventoryComponent::UseWeapon(FItemData WeaponData)
{
	switch (WeaponData.ItemName) {
	case EItemList::Dagger:
		GetOwner<ABoardController>()->Server_QueuWeapon(WeaponData, 1);
		break;
	default:
		break;
	}
}

void UInventoryComponent::Server_DropItem_Implementation(FItemData ItemData)
{
	ItemInventory.Remove(ItemData);

	GetOwner<ABoardController>()->DropItemOnRoom(ItemData);
}

void UInventoryComponent::DropAllItem()
{
	if (ItemInventory.IsEmpty())
		return;

	for (FItemData it : ItemInventory)
	{
		Server_DropItem(it);
	}
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryComponent, ItemInventory);
	DOREPLIFETIME(UInventoryComponent, bInventoryIsFull);
}
