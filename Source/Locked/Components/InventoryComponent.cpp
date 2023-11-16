// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"
#include "Locked/Character/PlayerControlPawn.h"
#include "Locked/Character/BoardController.h"
#include "Locked/Character/State/LockedPlayerState.h"
#include "Locked/GameState/LockedGameState.h"
#include "Locked/Room/RoomTile.h"
#include "Locked/BlueprintLibrary/LockedBFL.h"
#include "Kismet/GameplayStatics.h"
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

	//DEBUG for checking Inventory Items
	//for (int i = ItemInventory.Num() - 1; i >= 0; i--)
	//{
	//	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green, UEnum::GetValueAsString(ItemInventory[i].ItemName)+"_"+ FString::FromInt(ItemInventory[i].ItemIdNumber));
	//}
	//GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green, "Player Inventory:");
}

void UInventoryComponent::AddItemToInventory(FItemData NewItem)
{
	if (NewItem.ItemName == EItemList::Fist)
		return;

	ItemInventory.Add(NewItem);
	GetOwner<ABoardController>()->Server_GameModeUpdateInventoryInfo();
}

void UInventoryComponent::Server_RemoveItemFromInventory_Implementation(FItemData RemovedItem)
{
	ItemInventory.Remove(RemovedItem);
	GetOwner<ABoardController>()->Server_GameModeUpdateInventoryInfo();
}

void UInventoryComponent::UseConsumable(FItemData ConsumableData)
{
	switch (ConsumableData.ItemName) {
	case EItemList::EnergyDrink:
		GetOwner<ABoardController>()->GetPlayerState<ALockedPlayerState>()->DoubleMovePoint();
		break;
	case EItemList::UtilityKit:
		//GetOwner<ABoardController>()->GetPlayerState<ALockedPlayerState>()->TakeDamage(-1.0f, FDamageEvent(), nullptr, nullptr);
		GetOwner<ABoardController>()->TakeDamage(-1.0f, FDamageEvent(), nullptr, nullptr);
		break;
	case EItemList::SmokeBomb:
		GetOwner<ABoardController>()->GetBoardPlayerControl()->SmokeRoom();
	case EItemList::Key:
		GetOwner<ABoardController>()->Server_UseKeyOnMainRoom();
		break;
	case EItemList::Hammer:
		GetOwner<ABoardController>()->GetBoardPlayerControl()->Server_CheckWallForHammer(ConsumableData);
		return;
	case EItemList::Rifle:
		GetOwner<ABoardController>()->GetBoardPlayerControl()->Server_CheckRifleTargetAvailability();
		return;
	case EItemList::MonkeysPaw:
		GetOwner<ABoardController>()->Client_OpenPawDuplication(false);
		return;
	case EItemList::DarkEffigy:
		DestroyAllDroppedItem();
		break;
	default:
		break;
	}

	Server_RemoveItemFromInventory(ConsumableData);

	if (!ConsumableData.bIsTreasure)
	{
		ALockedGameState* GameState = GetWorld()->GetGameState<ALockedGameState>();
		GameState->Server_PutItemToPile(ConsumableData);
	}
}

void UInventoryComponent::UseWeapon(FItemData WeaponData)
{
	ABoardController* Controller = GetOwner<ABoardController>();

	switch (WeaponData.ItemName) {
	case EItemList::Dagger:
		Controller->Server_QueuWeapon(WeaponData, 1);
		break;
	case EItemList::Sword:
		Controller->Server_QueuWeapon(WeaponData, 2);
		break;
	case EItemList::Rifle:
		Controller->Server_QueuWeapon(WeaponData, 3);
		break;
	case EItemList::Hammer:
		Controller->Server_QueuWeapon(WeaponData, 2);
		break;
	case EItemList::MonkeysPaw:
		Controller->Client_OpenPawDuplication(true);
		break;
	case EItemList::SpikeArmor:
		Controller->Server_QueuWeapon(WeaponData, 4);
		break;
	default:
		break;
	}
}

void UInventoryComponent::UsePawConsumable(FItemData ConsumableData)
{
	switch (ConsumableData.ItemName) {
	case EItemList::EnergyDrink:
		GetOwner<ABoardController>()->GetPlayerState<ALockedPlayerState>()->DoubleMovePoint();
		break;
	case EItemList::UtilityKit:
		GetOwner<ABoardController>()->GetPlayerState<ALockedPlayerState>()->TakeDamage(-1.0f, FDamageEvent(), nullptr, nullptr);
		break;
	case EItemList::SmokeBomb:
		GetOwner<ABoardController>()->GetBoardPlayerControl()->SmokeRoom();
	case EItemList::Hammer:
		GetOwner<ABoardController>()->GetBoardPlayerControl()->Server_CheckWallForHammer(ConsumableData);
		return;
	default:
		break;
	}
}

void UInventoryComponent::UsePawWeapon(FItemData WeaponData)
{
	ABoardController* Controller = GetOwner<ABoardController>();

	FItemData MonkeyPawData;

	for (FItemData it : ItemInventory)
	{
		if (it.ItemName == EItemList::MonkeysPaw)
		{
			MonkeyPawData = it;
			break;
		}
	}

	switch (WeaponData.ItemName) {
	case EItemList::Dagger:
		Controller->Server_QueuWeapon(MonkeyPawData, 1);
		break;
	case EItemList::Sword:
		Controller->Server_QueuWeapon(MonkeyPawData, 2);
		break;
	case EItemList::Hammer:
		Controller->Server_QueuWeapon(MonkeyPawData, 2);
		break;
	default:
		break;
	}
}

void UInventoryComponent::UsedUpSpikeArmor_Implementation()
{
	for (FItemData it : ItemInventory)
	{
		if (it.ItemName == EItemList::SpikeArmor)
		{
			Server_RemoveItemFromInventory(it);

			return;
		}
	}
}

void UInventoryComponent::UsedUpRifle_Implementation()
{
	for (FItemData it : ItemInventory)
	{
		if (it.ItemName == EItemList::Rifle)
		{
			Server_RemoveItemFromInventory(it);

			return;
		}
	}
}

void UInventoryComponent::UsedUpMonkeyPaw_Implementation()
{
	for (FItemData it : ItemInventory)
	{
		if (it.ItemName == EItemList::MonkeysPaw)
		{
			Server_RemoveItemFromInventory(it);

			return;
		}
	}
}

bool UInventoryComponent::SearchForUtilityKit(FItemData& UtilityKit)
{
	for (FItemData it : ItemInventory)
	{
		if (it.ItemName == EItemList::UtilityKit)
		{
			UtilityKit = it;
			return true;
		}
	}

	return false;
}

void UInventoryComponent::DestroyAllDroppedItem_Implementation()
{
	TArray<AActor*> AllRoomActor;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARoomTile::StaticClass(), AllRoomActor);

	for (AActor* it : AllRoomActor)
	{
		ARoomTile* RoomTile = Cast<ARoomTile>(it);
		RoomTile->Server_DestroyDropItem();
	}
}

bool UInventoryComponent::HasSpikeArmor()
{
	for (FItemData it : ItemInventory)
	{
		if (it.ItemName == EItemList::SpikeArmor)
		{
			return true;
		}
	}

	return false;
}

void UInventoryComponent::Server_DropItem_Implementation(FItemData ItemData)
{
	Server_RemoveItemFromInventory(ItemData);

	GetOwner<ABoardController>()->DropItemOnRoom(ItemData);
}

void UInventoryComponent::DropAllItem_Implementation()
{
	while (!ItemInventory.IsEmpty())
	{
		Server_DropItem(ItemInventory[0]);
	}
	GetOwner<ABoardController>()->GetBoardPlayerControl()->Server_UpdateCurrentRoomDropItemData();
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryComponent, ItemInventory);
}
