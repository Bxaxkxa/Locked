// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerControlPawn.h"
#include "LockedCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "BoardController.h"
#include "MyProject/Room/MainRoomTile.h"
#include "MyProject/Room/RandomGenRoom.h"
#include "MyProject/GameState/LockedGameState.h"
#include "MyProject/GameMode/MyProjectGameMode.h"

#include "State/LockedPlayerState.h"
#include "Net/UnrealNetwork.h"

// Sets default values
APlayerControlPawn::APlayerControlPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a camera boom (pulls in towards the player if there is a collision)
	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("CameraBoom"));
	SetRootComponent(RootScene);

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(RootScene); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	SetReplicates(true);
	SetReplicateMovement(true);
}

void APlayerControlPawn::SpawnControlledCharacter()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		if (CharacterTemplate)
		{
			UWorld* World = GetWorld();
			if (World == nullptr)
			{
				return;
			}

			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = GetOwner();
			SpawnParams.Instigator = Cast<APawn>(GetOwner());
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			FTransform spawnlocation = GetActorTransform();
			FVector SpawnLocation = GetActorLocation();
			FRotator SpawnRotation = FRotator::ZeroRotator;

			ControlledCharacter = World->SpawnActor<ALockedCharacter>(CharacterTemplate, SpawnLocation, SpawnRotation, SpawnParams);

			SetActorLocation(CurrentRoom->GetActorLocation());

			ControlledCharacter->ControllerPawn = this;
		}
	}
}

// Called when the game starts or when spawned
void APlayerControlPawn::BeginPlay()
{
	Super::BeginPlay();

	if (GetLocalRole() == ROLE_Authority)
	{
		CurrentRoom = Cast<AMainRoomTile>(UGameplayStatics::GetActorOfClass(GetWorld(), AMainRoomTile::StaticClass()));

		SpawnControlledCharacter();

		CurrentRoom->AddIdlePlayer(ControlledCharacter);
		CurrentRoom->PlaceIdlePlayerAtIdlePosition();

		//FollowTarget = ControlledCharacter;

		RandomRoomGenDeck = Cast<ARandomGenRoom>(UGameplayStatics::GetActorOfClass(GetWorld(), ARandomGenRoom::StaticClass()));
	}
}

// Called every frame
void APlayerControlPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (FollowTarget)
	{
		FVector ResultTarget;

		ResultTarget = FMath::Lerp<FVector>(GetActorLocation(), FollowTarget->GetActorLocation(), DeltaTime * CameraFollowSpeed);

		SetActorLocation(ResultTarget);
	}
}

// Called to bind functionality to input
void APlayerControlPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void APlayerControlPawn::ManageMovement(ETileDirection MoveToDirection)
{

	switch (CurrentMovementInputState)
	{
	case EMovementInputState::E_CharMovement:
		Server_MoveCharacterTo(CurrentRoom, MoveToDirection);
		break;
	case EMovementInputState::E_TileMovement:
		MoveCameraTo(CurrentRoom->NeighbourRoom[static_cast<int>(MoveToDirection)].GetNextRoom());
		break;
	case EMovementInputState::E_TilePlacement:
		Server_MovePlacedTile(CurrentRoom->NeighbourRoom[static_cast<int>(MoveToDirection)], MoveToDirection);
		break;
	default:
		break;
	}
}

void APlayerControlPawn::MoveCameraTo(ARoomTile* MoveToTile)
{
	if (!MoveToTile)
		return;

	FollowTarget = MoveToTile;
	CurrentRoom = MoveToTile;
}

void APlayerControlPawn::MoveCharacterTo(ARoomTile* MoveToTile)
{
	if (!bStillInMove && MoveToTile)
	{
		bStillInMove = true;
		ControlledCharacter->CharacterMoveTo(MoveToTile);

		CurrentRoom = MoveToTile;
	}
}

void APlayerControlPawn::Server_MoveCharacterTo_Implementation(ARoomTile* MoveToTile, ETileDirection MoveToDirection)
{
	FDoorWay* CurrentRoomDoorWay = &MoveToTile->NeighbourRoom[static_cast<int>(MoveToDirection)];
	ARoomTile* MoveToRoom = CurrentRoomDoorWay->GetNextRoom();
	FDoorWay* OppositeRoomDoorWay = MoveToTile->GetOppositeDoorWay(MoveToDirection);

	ALockedPlayerState* State = GetPlayerState<ALockedPlayerState>();
	if (!bStillInMove && MoveToRoom &&
		CurrentRoomDoorWay->IsThereDoorway() && OppositeRoomDoorWay &&
		OppositeRoomDoorWay->IsThereDoorway() && State->AvailableMove > 0)
	{
		bStillInMove = true;
		ControlledCharacter->CharacterMoveTo(MoveToRoom);

		CurrentRoom = MoveToRoom;

		State->UseMovePoint();
	}
}

void APlayerControlPawn::MovePlacedTile(FDoorWay DoorWay, ETileDirection MoveToDirection)
{
	if (DoorWay.IsThereDoorway() && !DoorWay.GetNextRoom())
	{
		bool IsGoingLeft = MoveToDirection == ETileDirection::NextLeft;
		bool IsGoingRight = MoveToDirection == ETileDirection::NextRight;
		bool IsGoingUp = MoveToDirection == ETileDirection::NextUp;
		bool IsGoingDown = MoveToDirection == ETileDirection::NextDown;
		FVector AddPosition = FVector((IsGoingUp * 600.0f) + (IsGoingDown * -600.0f), (IsGoingRight * 600.0f) + (IsGoingLeft * -600.0f), 0.0f);
		DrawedRoomTile->SetActorLocation(CurrentRoom->GetActorLocation() + AddPosition);

		LastPlacedDirection = MoveToDirection;
		DrawedRoomTile->CheckOtherRoomConnectionAvailibility(LastPlacedDirection);
	}
}

void APlayerControlPawn::Server_MovePlacedTile_Implementation(FDoorWay DoorWay, ETileDirection MoveToDirection)
{
	if (DoorWay.IsThereDoorway() && !DoorWay.GetNextRoom())
	{
		bool IsGoingLeft = MoveToDirection == ETileDirection::NextLeft;
		bool IsGoingRight = MoveToDirection == ETileDirection::NextRight;
		bool IsGoingUp = MoveToDirection == ETileDirection::NextUp;
		bool IsGoingDown = MoveToDirection == ETileDirection::NextDown;
		FVector AddPosition = FVector((IsGoingUp * 600.0f) + (IsGoingDown * -600.0f), (IsGoingRight * 600.0f) + (IsGoingLeft * -600.0f), 0.0f);
		DrawedRoomTile->SetActorLocation(CurrentRoom->GetActorLocation() + AddPosition);

		LastPlacedDirection = MoveToDirection;
		DrawedRoomTile->CheckOtherRoomConnectionAvailibility(LastPlacedDirection);
	}
}

void APlayerControlPawn::ChangeCameraBehaviour(EMovementInputState NewInputState)
{
	CurrentMovementInputState = NewInputState;

	switch (NewInputState)
	{
	case EMovementInputState::E_CharMovement:
		FollowTarget = ControlledCharacter;
		break;
	case EMovementInputState::E_TileMovement:
		CurrentRoom = ControlledCharacter->CurrentRoom;
		FollowTarget = CurrentRoom;
		break;
	case EMovementInputState::E_TilePlacement:
		FollowTarget = DrawedRoomTile = RandomRoomGenDeck->DrawTile();
		break;
	default:
		break;
	}
}

void APlayerControlPawn::ChangeCameraPerspective(ALockedCharacter* CurrentTurnPlayerCharacter)
{
	FollowTarget = CurrentTurnPlayerCharacter;
}

void APlayerControlPawn::BackToActionMenu()
{
	ABoardController* BoardController = GetController<ABoardController>();
	if (BoardController && !bStillInMove && CurrentMovementInputState != EMovementInputState::E_TilePlacement)
	{
		BoardController->SetInputMode(FInputModeUIOnly());

		CurrentMovementInputState = EMovementInputState::E_Idle;
		FollowTarget = ControlledCharacter;
		BoardController->ShowActionWidget(true);
	}
}

void APlayerControlPawn::DrawRoomTile()
{
	if (!(CurrentMovementInputState == EMovementInputState::E_CharMovement) || bStillInMove || CurrentRoom->FullyConnected)
	{
		return;
	}

	FollowTarget = DrawedRoomTile = RandomRoomGenDeck->DrawTile();

	for (int i = 0; i < static_cast<int>(ETileDirection::NUM); i++)
	{
		if (CurrentRoom->NeighbourRoom[i].IsThereDoorway() && !CurrentRoom->NeighbourRoom[i].GetNextRoom())
		{
			bool IsGoingLeft = ETileDirection(i) == ETileDirection::NextLeft;
			bool IsGoingRight = ETileDirection(i) == ETileDirection::NextRight;
			bool IsGoingUp = ETileDirection(i) == ETileDirection::NextUp;
			bool IsGoingDown = ETileDirection(i) == ETileDirection::NextDown;
			FVector AddPosition = FVector((IsGoingUp * 600.0f) + (IsGoingDown * -600.0f), (IsGoingRight * 600.0f) + (IsGoingLeft * -600.0f), 0.0f);
			DrawedRoomTile->SetActorLocation(CurrentRoom->GetActorLocation() + AddPosition);
			LastPlacedDirection = ETileDirection(i);
			DrawedRoomTile->CheckOtherRoomConnectionAvailibility(LastPlacedDirection);
		}
	}

	CurrentMovementInputState = EMovementInputState::E_TilePlacement;

	ABoardController* BoardController = GetController<ABoardController>();
	if (BoardController)
		BoardController->Client_ChangeIndicatorLayout(EActionLayout::PlaceTileAction);
}

void APlayerControlPawn::PlaceRoomTile()
{
	if (!DrawedRoomTile)
	{
		return;
	}

	CurrentRoom->NeighbourRoom[static_cast<int>(LastPlacedDirection)].SetNextRoom(DrawedRoomTile);
	DrawedRoomTile->CheckNeightbourRooms();

	CurrentMovementInputState = EMovementInputState::E_CharMovement;

	Server_MoveCharacterTo(CurrentRoom, LastPlacedDirection);

	DrawedRoomTile = nullptr;

	FollowTarget = ControlledCharacter;

	ABoardController* BoardController = GetController<ABoardController>();
	if (BoardController)
		BoardController->Client_ChangeIndicatorLayout(EActionLayout::MoveAction);
}

void APlayerControlPawn::RotateRoomTile()
{
	if (DrawedRoomTile)
	{
		DrawedRoomTile->RotateRoomPlacement();
		DrawedRoomTile->CheckOtherRoomConnectionAvailibility(LastPlacedDirection);
	}
}

void APlayerControlPawn::DuelCheck(ALockedCharacter* DuelTarget)
{
	CurrentRoom->SetPlayerToDuelPosition(ControlledCharacter, true);
	CurrentRoom->SetPlayerToDuelPosition(DuelTarget, false);

	ABoardController* CurrentPlayerController = GetController<ABoardController>();
	ABoardController* DuelTargetController = DuelTarget->ControllerPawn->GetController<ABoardController>();

	CurrentPlayerController->Multicast_DisplayDuelOption(DuelTarget->ControllerPawn, true);
	DuelTargetController->Multicast_DisplayDuelOption(DuelTarget->ControllerPawn, false);

	AMyProjectGameMode* GameMode = (AMyProjectGameMode*)GetWorld()->GetAuthGameMode();
	GameMode->DuelSetup(CurrentPlayerController, DuelTargetController);
}

bool APlayerControlPawn::CheckRoomForDualTarget()
{
	if (!CurrentRoom->IdlePlayers.Num())
		return false;

	//Reset Idle Player Position
	CurrentRoom->PlaceIdlePlayerAtIdlePosition();

	if (NextDuelTarget >= CurrentRoom->IdlePlayers.Num())
	{
		NextDuelTarget = 0;
		ControlledCharacter->SetActorLocation(CurrentRoom->GetActorLocation());
		BackToActionMenu();
		return false;
	}

	DuelCheck(CurrentRoom->IdlePlayers[NextDuelTarget]);
	NextDuelTarget++;
	return true;
}

void APlayerControlPawn::DropItemOnCurrentRoom(FItemData ItemData)
{
	CurrentRoom->AddPendingDroppedItem(ItemData);
}

void APlayerControlPawn::UpdateCurrentRoomDropItemData()
{
	CurrentRoom->FillDroppedItemWithPendingDrop();
}

bool APlayerControlPawn::PickupItemOnCurrentRoom()
{
	if (!CurrentRoom->DroppedItem.Num())
		return false;

	FItemData PickedupItem = CurrentRoom->DroppedItem.Last();

	CurrentRoom->Server_PickedItemOnFloor(PickedupItem);
	GetController<ABoardController>()->Server_ObtainItem(PickedupItem, EObtainItemMethod::PickUp);
	return true;

}

void APlayerControlPawn::CheckMovePoint()
{
	ALockedPlayerState* State = GetPlayerState<ALockedPlayerState>();

	if (!State->AvailableMove)
	{
		BackToActionMenu();
	}
}

void APlayerControlPawn::StartTurn()
{
	ALockedPlayerState* State = GetPlayerState<ALockedPlayerState>();
	State->Server_RefreshMovePoint();

	CurrentRoom->RemovePlayerFromIdle(ControlledCharacter);

	CurrentRoom->PlaceIdlePlayerAtIdlePosition();
}

void APlayerControlPawn::EndTurn()
{
	ALockedPlayerState* State = GetPlayerState<ALockedPlayerState>();
	State->AvailableMove = 0;

	NextDuelTarget = 0;

	CurrentRoom->AddIdlePlayer(ControlledCharacter);
	CurrentRoom->PlaceIdlePlayerAtIdlePosition();
}

void APlayerControlPawn::Server_RotateRoomTile_Implementation()
{
	if (DrawedRoomTile)
	{
		DrawedRoomTile->RotateRoomPlacement();
		DrawedRoomTile->CheckOtherRoomConnectionAvailibility(LastPlacedDirection);
	}
}

void APlayerControlPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlayerControlPawn, ControlledCharacter);
	DOREPLIFETIME(APlayerControlPawn, RandomRoomGenDeck);
	DOREPLIFETIME(APlayerControlPawn, CurrentRoom);
	DOREPLIFETIME(APlayerControlPawn, DrawedRoomTile);
	DOREPLIFETIME(APlayerControlPawn, bStillInMove);
	DOREPLIFETIME(APlayerControlPawn, CurrentMovementInputState);
	DOREPLIFETIME(APlayerControlPawn, FollowTarget);
	DOREPLIFETIME(APlayerControlPawn, CameraFollowSpeed);
}