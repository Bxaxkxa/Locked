// Fill out your copyright notice in the Description page of Project Settings.


#include "RoomTile.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Locked/Components/RoomTileWallMesh.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Locked/Character/LockedCharacter.h"
#include "Locked/Character/PlayerControlPawn.h"
#include "Locked/Character/BoardController.h"
#include "Locked/Widget/RoomCount.h"
#include "NiagaraComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ARoomTile::ARoomTile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RoomDetectionCollider = CreateDefaultSubobject<UBoxComponent>("RoomDetectionCollider");
	RoomDetectionCollider->SetCollisionProfileName("OverlapOnlyPawn");
	SetRootComponent(RoomDetectionCollider);

	FloorMesh = CreateDefaultSubobject<UStaticMeshComponent>("FloorMesh");
	FloorMesh->SetupAttachment(RootComponent);
	FloorMesh->SetIsReplicated(true);

	WallMesh = CreateDefaultSubobject<UStaticMeshComponent>("WallMesh");
	WallMesh->SetupAttachment(FloorMesh);

	RoomCountWidget = CreateDefaultSubobject<UWidgetComponent>("RoomCountWidget");
	RoomCountWidget->SetDrawAtDesiredSize(true);
	RoomCountWidget->SetRelativeLocation(FVector(0, 0, 0.5));
	RoomCountWidget->SetRelativeRotation(FRotator(90, 180, 0));
	RoomCountWidget->SetupAttachment(RootComponent);
	RoomCountWidget->SetIsReplicated(true);
	RoomCountWidget->SetCastShadow(false);

	ParticleSystem = CreateDefaultSubobject<UNiagaraComponent>("Particle");
	ParticleSystem->SetupAttachment(RootComponent);
	ParticleSystem->SetIsReplicated(true);

	SetReplicates(true);
	SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void ARoomTile::BeginPlay()
{
	Super::BeginPlay();

	//Top Right
	PlayerIdlePositions[0] = FVector(PlayerIdleDistance, PlayerIdleDistance, 0);
	//Top Left
	PlayerIdlePositions[1] = FVector(PlayerIdleDistance, -PlayerIdleDistance, 0);
	//Bottom Left
	PlayerIdlePositions[2] = FVector(-PlayerIdleDistance, -PlayerIdleDistance, 0);
	//Bottom Right
	PlayerIdlePositions[3] = FVector(-PlayerIdleDistance, PlayerIdleDistance, 0);
}

// Called every frame
void ARoomTile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ARoomTile::CheckNeightbourRooms()
{
	for (int i = 0; i < static_cast<int>(ETileDirection::NUM); i++)
	{
		if (NeighbourRoom[i].GetNextRoom())
		{
			continue;
		}

		float DirAngleInRad = 90.0f * i / 180.0f * PI;
		FVector Direction = FVector(sinf(DirAngleInRad), cosf(DirAngleInRad), 0.0f);
		FVector AddPosition = Direction * 200.0f;
		FVector TraceStart = GetActorLocation() + AddPosition;

		FVector TraceEnd = TraceStart + Direction * 200.0f;

		FCollisionQueryParams TraceParams;
		TraceParams.AddIgnoredActor(this);
		TraceParams.bTraceComplex = true;
		TraceParams.bReturnPhysicalMaterial = false;

		UWorld* World = GetWorld();
		FHitResult OutHit;

		if (!World->LineTraceSingleByChannel(OutHit, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility, TraceParams))
		{
			continue;
		}
		ARoomTile* HitRoom = Cast<ARoomTile>(OutHit.GetActor());

		NeighbourRoom[i].SetNextRoom(HitRoom);
		GetOppositeDoorWay(ETileDirection(i))->SetNextRoom(this);
		FullyConnected = IsDoorWaysFullyFilled();
		HitRoom->FullyConnected = HitRoom->IsDoorWaysFullyFilled();
	}
}

void ARoomTile::SetOppositeRoom(ETileDirection OriginDirection, ARoomTile* OriginRoom)
{
	ETileDirection OppositeDirection = GetOppositeDirection(OriginDirection);

	NeighbourRoom[(int)OppositeDirection].SetNextRoom(OriginRoom);
}

void ARoomTile::Server_RotateRoomPlacement_Implementation()
{
	FDoorWay LastDoorWayReplacement = NeighbourRoom[(int)ETileDirection::NextRight];

	for (int i = 0; i < (int)ETileDirection::NextDown; i++)
	{
		NeighbourRoom[i] = NeighbourRoom[i + 1];
	}

	NeighbourRoom[(int)ETileDirection::NextDown] = LastDoorWayReplacement;

	FRotator NewRotation = FloorMesh->GetRelativeRotation() + FRotator(0.0f, 90.0f, 0.0f);
	//SetActorRotation(NewRotation);
	FloorMesh->SetRelativeRotation(NewRotation);
}

void ARoomTile::CheckOtherRoomConnectionAvailability(ETileDirection OriginDirection)
{
	ETileDirection OppositeDirection = GetOppositeDirection(OriginDirection);

	while (!NeighbourRoom[(int)OppositeDirection].IsThereDoorway())
	{
		Server_RotateRoomPlacement();
	}
}

bool ARoomTile::IsDoorWaysFullyFilled()
{
	for (FDoorWay it : NeighbourRoom)
	{
		if (!it.IsThereDoorway())
			continue;
		if (!it.GetNextRoom())
			return false;
	}
	return true;
}

FVector ARoomTile::GetNextAvailableIdleSpot()
{
	FVector PlayerIdlePositionTarget = GetActorLocation() + PlayerIdlePositions[0];
	return PlayerIdlePositionTarget;
}

void ARoomTile::AddIdlePlayer(ALockedCharacter* PlayerCharacter)
{
	IdlePlayers.Add(PlayerCharacter);

	PlayerCharacter->ChangeToIdleCollision(true);
}

void ARoomTile::RemovePlayerFromIdle(ALockedCharacter* PlayerCharacter)
{
	if (PlayerCharacter && IdlePlayers.Contains(PlayerCharacter))
	{
		PlayerCharacter->SetActorLocation(GetActorLocation() + FVector(0, 0, PlayerCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()));

		IdlePlayers.Remove(PlayerCharacter);

		PlayerCharacter->ChangeToIdleCollision(false);
	}
}

void ARoomTile::PlaceIdlePlayerAtIdlePosition()
{
	for (int i = 0; i < IdlePlayers.Num(); i++)
	{
		FVector RoomIdlePosition = GetActorLocation() + PlayerIdlePositions[i] + FVector(0, 0, IdlePlayers[i]->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());

		IdlePlayers[i]->SetActorLocation(RoomIdlePosition);
		IdlePlayers[i]->SetActorRotation(FRotator(0.0f, 180.0f, 0.0f));
	}
}

void ARoomTile::SetPlayerToDuelPosition(ALockedCharacter* PlayerCharacter, bool IsAttacker)
{
	float AddYPosition = DuelDistance * !IsAttacker + (IsAttacker * -1) * DuelDistance;
	FVector DuelPosition = GetActorLocation() + FVector(0, AddYPosition, 0);
	PlayerCharacter->SetActorLocation(DuelPosition);


	FRotator DuelRotation = FRotator(0, 90.0f * IsAttacker + (!IsAttacker * -1) * 90.0f, 0);
	PlayerCharacter->SetActorRotation(DuelRotation);
}

void ARoomTile::AddPendingDroppedItem(FItemData NewDroppedItem)
{
	PendingDroppedItem.Add(NewDroppedItem);
}

void ARoomTile::FillDroppedItemWithPendingDrop()
{
	for (FItemData it : PendingDroppedItem)
	{
		DroppedItem.Add(it);
	}

	PendingDroppedItem.Empty();

	Multicast_UpdateWidgetCount(DroppedItem.Num());
}

bool ARoomTile::RoomEvent(APlayerControlPawn* Player)
{
	if (TreasureCheck(Player))
		return true;

	if (DroppedItem.Num() != 0)
	{
		Server_PickupItemCheck(Player);
		return true;
	}

	if (DuelTargetCheck(Player))
		return true;

	return false;
}

void ARoomTile::SmokeRoom_Implementation(bool bState)
{
	bIsSmoked = bState;

	bState ? ParticleSystem->ActivateSystem() : ParticleSystem->Deactivate();

	SmokeDuration = SmokeMaxDuration * bState;
}

void ARoomTile::DestroyWall_Implementation(bool bState, ETileDirection WallDirection)
{
	TArray<UActorComponent*> OutArray = GetComponentsByClass(URoomTileWallMesh::StaticClass());

	for (UActorComponent* it : OutArray)
	{
		URoomTileWallMesh* BreakWallMesh = Cast<URoomTileWallMesh>(it);
		if (BreakWallMesh->IsDoorOfDirection(WallDirection))
		{
			BreakWallMesh->Break(bState);
			break;
		}
	}

	NeighbourRoom[static_cast<int>(WallDirection)].BreakWall(bState);
}

bool ARoomTile::DuelTargetCheck(APlayerControlPawn* Player)
{
	if (!IdlePlayers.Num())
		return false;
	//Reset Idle Player Position
	PlaceIdlePlayerAtIdlePosition();

	if (NextDuelTarget >= IdlePlayers.Num())
	{
		NextDuelTarget = 0;
		Player->GetPlayerCharacter()->SetActorLocation(GetActorLocation());
		Player->BackToActionMenu();
		return false;
	}

	Player->DuelCheck(IdlePlayers[NextDuelTarget]);
	NextDuelTarget++;
	return true;
}

bool ARoomTile::TreasureCheck(APlayerControlPawn* Player)
{
	if (bIsTreasureRoom && !bIsTreasureLooted)
	{
		ABoardController* CurrentPlayerController = Player->GetController<ABoardController>();

		CurrentPlayerController->DrawTreasureFromDeck();

		return bIsTreasureLooted = true;
	}

	return false;
}

void ARoomTile::Server_PickupItemCheck_Implementation(APlayerControlPawn* Player)
{
	FItemData PickedupItem = DroppedItem.Last();

	if (PickedupItem.ItemName == EItemList::Fist)
	{
		return;
	}

	Server_PickedItemOnFloor(PickedupItem);

	ABoardController* CurrentPlayerController = Player->GetController<ABoardController>();
	CurrentPlayerController->Server_ObtainItem(PickedupItem, EObtainItemMethod::PickUp);
}

void ARoomTile::Server_UpdateRoomState_Implementation()
{
	SmokeDuration--;

	if (SmokeDuration == 0)
	{
		SmokeRoom(false);
	}
}

void ARoomTile::Multicast_UpdateWidgetCount_Implementation(int UpdateCountNumber)
{
	UUserWidget* WidgetObject = RoomCountWidget->GetUserWidgetObject();
	URoomCount* CountWidget = Cast<URoomCount>(WidgetObject);

	CountWidget->UpdateCount(UpdateCountNumber);
}

void ARoomTile::Server_PickedItemOnFloor(FItemData PickedItem)
{
	DroppedItem.Remove(PickedItem);
	Multicast_UpdateWidgetCount(DroppedItem.Num());
}

void ARoomTile::Server_DestroyDropItem_Implementation()
{
	int CountGoal = 0;
	for (int i = 0; DroppedItem.Num() > CountGoal;)
	{
		if (DroppedItem[i].bIsTreasure)
		{
			CountGoal = ++i;
			continue;
		}
		DroppedItem.RemoveAt(i);
	}

	Multicast_UpdateWidgetCount(DroppedItem.Num());
}

bool ARoomTile::IsOppositeRoomConnected(ETileDirection OriginDirection)
{
	if (!NeighbourRoom[static_cast<int>(OriginDirection)].GetNextRoom())
	{
		return false;
	}

	return NeighbourRoom[static_cast<int>(OriginDirection)].IsThereDoorway() && GetOppositeDoorWay(OriginDirection)->IsThereDoorway();;
}

FDoorWay* ARoomTile::GetOppositeDoorWay(ETileDirection OriginDirection)
{
	ETileDirection OppositeDirection = GetOppositeDirection(OriginDirection);

	ARoomTile* OppositeRoom = NeighbourRoom[static_cast<int>(OriginDirection)].GetNextRoom();
	return &OppositeRoom->NeighbourRoom[static_cast<int>(OppositeDirection)];
}

ETileDirection ARoomTile::GetOppositeDirection(ETileDirection OriginDirection)
{
	ETileDirection OppositeDirection;
	int DirectionEnumBit = static_cast<int>(OriginDirection);
	if (DirectionEnumBit >= 2)
	{
		OppositeDirection = ETileDirection(DirectionEnumBit - 2);
	}
	else
	{
		OppositeDirection = ETileDirection(DirectionEnumBit + 2);
	}

	return OppositeDirection;
}

void ARoomTile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARoomTile, FloorMesh);
	DOREPLIFETIME(ARoomTile, RoomCountWidget);
	DOREPLIFETIME(ARoomTile, NeighbourRoom);
	DOREPLIFETIME(ARoomTile, IdlePlayers);
	DOREPLIFETIME(ARoomTile, PlayerIdlePositions);
	DOREPLIFETIME(ARoomTile, DroppedItem);
	DOREPLIFETIME(ARoomTile, PendingDroppedItem);
	DOREPLIFETIME(ARoomTile, bIsTreasureLooted);
	DOREPLIFETIME(ARoomTile, bIsSmoked);
	DOREPLIFETIME(ARoomTile, SmokeDuration);
	DOREPLIFETIME(ARoomTile, FullyConnected);
}
