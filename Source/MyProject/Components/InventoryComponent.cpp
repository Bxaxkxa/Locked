// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"
#include "MyProject/Character/PlayerControlPawn.h"
#include "MyProject/Character/BoardController.h"
#include "MyProject/Character/State/LockedPlayerState.h"
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

}

void UInventoryComponent::AddItemToInventory(FItemData NewItem)
{
	ItemInventory.Add(NewItem);
}

//void UInventoryComponent::RemoveItemFromInventory_Implementation(FItemData RemovedItem)
void UInventoryComponent::RemoveItemFromInventory(FItemData RemovedItem)
{
	ItemInventory.Remove(RemovedItem);
}

void UInventoryComponent::UseConsumable(FItemData ConsumableData)
{
	switch (ConsumableData.ItemName){
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

	////ItemInventory.Remove(WeaponData);
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryComponent, ItemInventory);
}
