// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerControlPawn.h"
#include "LockedCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "BoardController.h"
#include "Locked/Room/MainRoomTile.h"
#include "Locked/Room/RandomGenRoom.h"
#include "Locked/GameState/LockedGameState.h"
#include "Locked/GameMode/MyProjectGameMode.h"
#include "Locked/Components/LockedFollowCamera.h"
#include "Locked/Components/InventoryComponent.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
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
	FollowCamera = CreateDefaultSubobject<ULockedFollowCamera>(TEXT("FollowCamera"));
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

	/*FString BoolStr = bStillInMove ? "true" : "false";
	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green, "IsPlayerStillMoving? " + BoolStr);*/

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
	case EMovementInputState::E_MoveTileMovement:
	case EMovementInputState::E_TileMovement:
		MoveCameraTo(ViewMapTarget->NeighbourRoom[static_cast<int>(MoveToDirection)].GetNextRoom());
		break;
	case EMovementInputState::E_TilePlacement:
		Server_MovePlacedTile(CurrentRoom->NeighbourRoom[static_cast<int>(MoveToDirection)], MoveToDirection);
		break;
	case EMovementInputState::E_RifleTarget:
		if (MoveToDirection == ETileDirection::NextUp || MoveToDirection == ETileDirection::NextRight)
			NextRifleTarget();
		else
			PrevRifleTarget();
		break;
	case EMovementInputState::E_HammerTarget:
		ChangeHammerDirection(MoveToDirection);
		break;
	default:
		break;
	}
}

void APlayerControlPawn::Confirm()
{
	if (CurrentMovementInputState == EMovementInputState::E_TilePlacement)
		PlaceRoomTile();

	if (CurrentMovementInputState == EMovementInputState::E_RifleTarget)
	{
		ShootRifle();
	}

	if (CurrentMovementInputState == EMovementInputState::E_HammerTarget)
	{
		DestroyWall();
	}
}

void APlayerControlPawn::MoveCameraTo(ARoomTile* MoveToTile)
{
	if (!MoveToTile)
		return;

	FollowTarget = MoveToTile;
	ViewMapTarget = MoveToTile;
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
	if (!bStillInMove && MoveToRoom && !MoveToRoom->bIsSmoked &&
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
		int DirEnumIndex = static_cast<int>(MoveToDirection);
		float DirAngleInRad = 90.0f * DirEnumIndex / 180.0f * PI;

		FVector AddPosition = FVector(sinf(DirAngleInRad) * 600.0f, cosf(DirAngleInRad) * 600.0f, 0.0f);
		DrawedRoomTile->SetActorLocation(CurrentRoom->GetActorLocation() + AddPosition);

		LastPlacedDirection = MoveToDirection;
		DrawedRoomTile->CheckOtherRoomConnectionAvailability(LastPlacedDirection);
	}
}

void APlayerControlPawn::Server_MovePlacedTile_Implementation(FDoorWay DoorWay, ETileDirection MoveToDirection)
{
	if (DoorWay.IsThereDoorway() && !DoorWay.GetNextRoom())
	{
		int DirEnumIndex = static_cast<int>(MoveToDirection);

		float DirAngleInRad = 90.0f * DirEnumIndex / 180.0f * PI;

		FVector AddPosition = FVector(sinf(DirAngleInRad) * 600.0f, cosf(DirAngleInRad) * 600.0f, 0.0f);
		DrawedRoomTile->SetActorLocation(CurrentRoom->GetActorLocation() + AddPosition);

		LastPlacedDirection = MoveToDirection;
		DrawedRoomTile->CheckOtherRoomConnectionAvailability(LastPlacedDirection);
	}
}

void APlayerControlPawn::ChangeCameraBehaviour_Implementation(EMovementInputState NewInputState)
{
	CurrentMovementInputState = NewInputState;

	switch (NewInputState)
	{
	case EMovementInputState::E_Idle:
	case EMovementInputState::E_CharMovement:
		FollowTarget = ControlledCharacter;
		FollowCamera->ChangeCameraAngle(CameraFollowAngle);
		break;
	case EMovementInputState::E_TileMovement:
	case EMovementInputState::E_MoveTileMovement:
		ViewMapTarget = ControlledCharacter->CurrentRoom;
		FollowCamera->ChangeCameraAngle(CameraViewMapAngle);
		FollowTarget = CurrentRoom;
		break;
	case EMovementInputState::E_TilePlacement:
		//FollowTarget = DrawedRoomTile = RandomRoomGenDeck->DrawTile();
		if (!DrawedRoomTile)
			ChangeCameraBehaviour(EMovementInputState::E_CharMovement);
		break;
	case EMovementInputState::E_RifleTarget:
		break;
	default:
		break;
	}
}

void APlayerControlPawn::ChangeCameraPerspective_Implementation(ALockedCharacter* CurrentTurnPlayerCharacter)
{
	FollowTarget = CurrentTurnPlayerCharacter;
}

void APlayerControlPawn::HandleBack()
{
	ABoardController* BoardController = GetController<ABoardController>();

	switch (CurrentMovementInputState)
	{
	case EMovementInputState::E_CharMovement:
	case EMovementInputState::E_TileMovement:
		BackToActionMenu();
		break;
	case EMovementInputState::E_MoveTileMovement:
		ChangeCameraBehaviour(EMovementInputState::E_CharMovement);
		BoardController->Client_ChangeIndicatorLayout(EActionLayout::MoveAction);
		BoardController->Client_ShowMovePointIndicator(true);
		break;
	case EMovementInputState::E_TilePlacement:
		break;
	case EMovementInputState::E_RifleTarget:
		BackFromRifleState();
		break;
	case EMovementInputState::E_HammerTarget:
		BackFromHammerState();
		break;
	default:
		break;
	}
}

void APlayerControlPawn::BackToActionMenu()
{
	ABoardController* BoardController = GetController<ABoardController>();
	if (!BoardController)
		return;

	if (!bStillInMove)
	{
		ChangeCameraBehaviour(EMovementInputState::E_Idle);

		BoardController->Client_SetControllerInputMode(false);

		BoardController->Client_ChangeIndicatorLayout(EActionLayout::MenuAction);
		BoardController->Client_ShowMovePointIndicator(false);

		//CurrentMovementInputState = EMovementInputState::E_Idle;
		//FollowTarget = ControlledCharacter;
		//FollowCamera->ChangeCameraAngle(CameraFollowAngle);
		BoardController->ShowActionWidget(true);
	}
}

void APlayerControlPawn::BackToInventory()
{
	ABoardController* BoardController = GetController<ABoardController>();
	if (!BoardController)
		return;

	ChangeCameraBehaviour(EMovementInputState::E_Idle);

	BoardController->Client_ShowInventoryWidget();
}

void APlayerControlPawn::BackFromRifleState()
{
	ABoardController* BoardController = GetController<ABoardController>();

	if (!BoardController)
		return;
	BoardController->Client_ChangeIndicatorLayout(EActionLayout::ItemAction);

	BackToInventory();
}

void APlayerControlPawn::BackFromHammerState()
{
	ControlledCharacter->SetActorLocation(CurrentRoom->GetActorLocation() + FVector(0, 0, ControlledCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()));

	BackToInventory();
}

void APlayerControlPawn::ChangeToRifleStance()
{
	FollowTarget = CurrentRifleTargets = RifleTargets[0];
	CurrentRifleTargetsIndex = 0;
	ChangeCameraBehaviour(EMovementInputState::E_RifleTarget);

	ABoardController* BoardController = GetController<ABoardController>();
	if (BoardController)
	{
		BoardController->Client_RemoveInventoryWidget();
		BoardController->Client_SetControllerInputMode(true);
	}
}

void APlayerControlPawn::NextRifleTarget_Implementation()
{
	CurrentRifleTargetsIndex = CurrentRifleTargetsIndex + 1 % RifleTargets.Num();
	FollowTarget = CurrentRifleTargets = RifleTargets[CurrentRifleTargetsIndex];
}

void APlayerControlPawn::PrevRifleTarget_Implementation()
{
	CurrentRifleTargetsIndex = (!CurrentRifleTargetsIndex * (RifleTargets.Num() - 1)) + ((CurrentRifleTargetsIndex - 1) * (bool)CurrentRifleTargetsIndex);
	FollowTarget = CurrentRifleTargets = RifleTargets[CurrentRifleTargetsIndex];
}

void APlayerControlPawn::Server_CheckRifleTargetAvailability_Implementation()
{
	RifleTargets.Empty();

	int DirectionIndex = 0;
	for (int i = 0; i < static_cast<int>(ETileDirection::NUM); i++)
	{
		ETileDirection CheckDirection = ETileDirection(i);

		ARoomTile* CheckedTile = CurrentRoom;
		//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, "Direction Check");
		for (int count = 1; count < 3; count++)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, "Distance Check");
			FDoorWay CheckDoorWay = CheckedTile->NeighbourRoom[static_cast<int>(CheckDirection)];
			if (!CheckDoorWay.IsThereDoorway() || !CheckDoorWay.GetNextRoom())
			{
				break;
			}

			CheckedTile = CheckDoorWay.GetNextRoom();
			if (CheckedTile->StaticClass() == AMainRoomTile::StaticClass())
			{
				continue;
			}

			for (ALockedCharacter* it : CheckedTile->IdlePlayers)
			{
				RifleTargets.Add(it);
			}
		}
	}

	if (RifleTargets.Num())
	{
		ChangeToRifleStance();
		return;
	}
	ABoardController* BoardController = GetController<ABoardController>();
	if (BoardController)
	{
		BoardController->Client_ShowTextMessage("No available target found");
	}
}


void APlayerControlPawn::DrawRoomTile()
{
	ALockedPlayerState* State = GetPlayerState<ALockedPlayerState>();

	if (!(CurrentMovementInputState == EMovementInputState::E_CharMovement) || bStillInMove || CurrentRoom->FullyConnected || !State->AvailableMove)
		return;

	DrawedRoomTile = RandomRoomGenDeck->DrawTile();
	//TODO: Add No More Tile Message
	if (!DrawedRoomTile)
		return;

	FollowTarget = DrawedRoomTile;
	ChangeCameraBehaviour(EMovementInputState::E_TilePlacement);

	for (int i = 0; i < static_cast<int>(ETileDirection::NUM); i++)
	{
		if (CurrentRoom->NeighbourRoom[i].IsThereDoorway() && !CurrentRoom->NeighbourRoom[i].GetNextRoom())
		{
			float DirAngleInRad = 90.0f * i / 180.0f * PI;

			FVector AddPosition = FVector(sinf(DirAngleInRad) * 600.0f, cosf(DirAngleInRad) * 600.0f, 0.0f);
			DrawedRoomTile->SetActorLocation(CurrentRoom->GetActorLocation() + AddPosition);
			LastPlacedDirection = ETileDirection(i);
			DrawedRoomTile->CheckOtherRoomConnectionAvailability(LastPlacedDirection);
		}
	}

	ABoardController* BoardController = GetController<ABoardController>();
	if (BoardController)
		BoardController->Client_ChangeIndicatorLayout(EActionLayout::PlaceTileAction);
}

void APlayerControlPawn::PlaceRoomTile()
{
	if (!DrawedRoomTile)
		return;

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
		DrawedRoomTile->Server_RotateRoomPlacement();
		DrawedRoomTile->CheckOtherRoomConnectionAvailability(LastPlacedDirection);
	}
}

void APlayerControlPawn::ShootRifle_Implementation()
{
	APlayerControlPawn* TargetControlPawn = CurrentRifleTargets->ControllerPawn;
	ABoardController* TargetPawnController = TargetControlPawn->GetController<ABoardController>();

	if (!TargetPawnController)
		return;

	//TODO:Add Spike Armor reaction
	UInventoryComponent* TargetInventory = TargetPawnController->GetInventory();
	bool HasSpikeArmor = TargetInventory->HasSpikeArmor();
	if (HasSpikeArmor)
	{
		TargetInventory->UsedUpSpikeArmor();
	}

	TargetPawnController->TakeDamage(1.0f * !HasSpikeArmor, FDamageEvent(), nullptr, nullptr);
	GetController<ABoardController>()->UsedUpRifle();

	BackFromRifleState();
}

void APlayerControlPawn::SmokeRoom_Implementation()
{
	CurrentRoom->SmokeRoom(true);
}

void APlayerControlPawn::Server_CheckWallForHammer_Implementation(FItemData ItemCard)
{
	for (int i = 0; i < static_cast<int>(ETileDirection::NUM); i++)
	{
		ETileDirection CheckDirection = ETileDirection(i);
		ETileDirection OppositeDirection = ARoomTile::GetOppositeDirection(CheckDirection);

		FDoorWay CheckDoorWay = CurrentRoom->NeighbourRoom[static_cast<int>(CheckDirection)];

		if (CheckDoorWay.IsThereDoorway())
		{
			ARoomTile* OppositeRoom = CurrentRoom->NeighbourRoom[static_cast<int>(CheckDirection)].GetNextRoom();
			if (!OppositeRoom)
			{
				continue;
			}

			FDoorWay CheckOppositeDoorWay = OppositeRoom->NeighbourRoom[static_cast<int>(OppositeDirection)];
			if (CheckOppositeDoorWay.IsThereDoorway())
			{
				continue;
			}
		}
		//Start Reaction
		UsedHammer = ItemCard;
		ChangeToHammerStance(CheckDirection);
		return;
	}

	ABoardController* BoardController = GetController<ABoardController>();
	if (BoardController)
	{
		BoardController->Client_ShowTextMessage("No available target found");
	}
}

void APlayerControlPawn::ChangeToHammerStance(ETileDirection StartingDir)
{
	CurrentMovementInputState = EMovementInputState::E_HammerTarget;

	CurrentHammerDirection = StartingDir;

	FDoorWay CheckDoorWay = CurrentRoom->NeighbourRoom[static_cast<int>(StartingDir)];

	float DirAngleInRad = 90.0f * static_cast<int>(StartingDir) / 180.0f * PI;
	FVector AddPosition = FVector(sinf(DirAngleInRad) * 200, cosf(DirAngleInRad) * 200, 0.0f);
	ControlledCharacter->SetActorLocation(CurrentRoom->GetActorLocation() + AddPosition);

	ABoardController* BoardController = GetController<ABoardController>();
	if (BoardController)
	{
		BoardController->Client_RemoveInventoryWidget();
		BoardController->Client_SetControllerInputMode(true);
	}
}

void APlayerControlPawn::DestroyWall_Implementation()
{
	GetController<ABoardController>()->Server_UsedUpWeapon(UsedHammer);
	UsedHammer = FItemData();

	CurrentRoom->DestroyWall(true, CurrentHammerDirection);

	ARoomTile* OppositeRoom = CurrentRoom->NeighbourRoom[static_cast<int>(CurrentHammerDirection)].GetNextRoom();
	if (OppositeRoom)
	{
		OppositeRoom->DestroyWall(true, ARoomTile::GetOppositeDirection(CurrentHammerDirection));
	}

	CheckMoveAvailability();

	BackFromHammerState();
}

void APlayerControlPawn::ChangeHammerDirection_Implementation(ETileDirection NewDirection)
{
	FDoorWay CheckDoorWay = CurrentRoom->NeighbourRoom[static_cast<int>(NewDirection)];
	ETileDirection OppositeDirection = ARoomTile::GetOppositeDirection(NewDirection);

	if (CheckDoorWay.IsThereDoorway())
	{
		ARoomTile* OppositeRoom = CurrentRoom->NeighbourRoom[static_cast<int>(NewDirection)].GetNextRoom();
		if (!OppositeRoom)
		{
			return;
		}

		FDoorWay CheckOppositeDoorWay = OppositeRoom->NeighbourRoom[static_cast<int>(OppositeDirection)];
		if (CheckOppositeDoorWay.IsThereDoorway())
		{
			return;
		}
	}

	CurrentHammerDirection = NewDirection;

	float DirAngleInRad = 90.0f * static_cast<int>(NewDirection) / 180.0f * PI;
	FVector AddPosition = FVector(sinf(DirAngleInRad) * 200, cosf(DirAngleInRad) * 200, 0.0f);
	ControlledCharacter->SetActorLocation(CurrentRoom->GetActorLocation() + AddPosition);
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
	return CurrentRoom->DuelTargetCheck(this);
}

bool APlayerControlPawn::CheckTreasureRoom()
{
	if (CurrentRoom->bIsTreasureRoom && !CurrentRoom->bIsTreasureLooted)
	{
		ABoardController* CurrentPlayerController = GetController<ABoardController>();

		CurrentPlayerController->DrawTreasureFromDeck();
		return true;
	}

	return false;
}

bool APlayerControlPawn::RunRoomEvent()
{
	return CurrentRoom->RoomEvent(this);
}

void APlayerControlPawn::DropItemOnCurrentRoom(FItemData ItemData)
{
	if (!Cast<AMainRoomTile>(CurrentRoom))
		CurrentRoom->AddPendingDroppedItem(ItemData);
}

void APlayerControlPawn::Server_UpdateCurrentRoomDropItemData_Implementation()
{
	if (!Cast<AMainRoomTile>(CurrentRoom))
		CurrentRoom->FillDroppedItemWithPendingDrop();
}

bool APlayerControlPawn::PickupItemOnCurrentRoom()
{
	if (!CurrentRoom->DroppedItem.Num())
		return false;

	CurrentRoom->Server_PickupItemCheck(this);
	return true;
}

void APlayerControlPawn::UseKeyAtMainRoom()
{
	AMainRoomTile* MainRoom = Cast<AMainRoomTile>(CurrentRoom);
	if (MainRoom)
	{
		MainRoom->AddKey();
	}
}

void APlayerControlPawn::CheckMoveAvailability_Implementation()
{
	for (int i = 0; i < static_cast<int>(ETileDirection::NUM); i++)
	{
		ETileDirection CheckDirection = ETileDirection(i);
		FDoorWay CheckedDoorWay = CurrentRoom->NeighbourRoom[i];

		//If next room is nullptr and still a door way to connect
		//Or if current room and next room is connected and next room not smoked
		if ((!CheckedDoorWay.GetNextRoom() && CheckedDoorWay.IsThereDoorway()) ||
			(CurrentRoom->IsOppositeRoomConnected(CheckDirection) && !CheckedDoorWay.GetNextRoom()->bIsSmoked))
		{
			bIsOutOfMove = false;
			return;
		}
	}
	bIsOutOfMove = true;
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

	CheckMoveAvailability();

	CurrentRoom->RemovePlayerFromIdle(ControlledCharacter);

	if (!State->GetHealthPoint())
	{
		//Revive/Respawn
		ControlledCharacter->Respawn();

		State->ResetHealthPoint();

		AMyProjectGameMode* GameMode = Cast<AMyProjectGameMode>(GetWorld()->GetAuthGameMode());
		GameMode->UpdatePlayerHealthInfo();
	}

	CurrentRoom->PlaceIdlePlayerAtIdlePosition();
}

void APlayerControlPawn::EndTurn()
{
	ALockedPlayerState* State = GetPlayerState<ALockedPlayerState>();
	State->AvailableMove = 0;
	bStillInMove = false;

	CurrentRoom->NextDuelTarget = 0;

	CurrentRoom->AddIdlePlayer(ControlledCharacter);
	CurrentRoom->PlaceIdlePlayerAtIdlePosition();
}

void APlayerControlPawn::Server_RotateRoomTile_Implementation()
{
	if (DrawedRoomTile)
	{
		DrawedRoomTile->Server_RotateRoomPlacement();
		DrawedRoomTile->CheckOtherRoomConnectionAvailability(LastPlacedDirection);
	}
}

void APlayerControlPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlayerControlPawn, ControlledCharacter);
	DOREPLIFETIME(APlayerControlPawn, RandomRoomGenDeck);
	DOREPLIFETIME(APlayerControlPawn, CurrentRoom);
	DOREPLIFETIME(APlayerControlPawn, ViewMapTarget);
	DOREPLIFETIME(APlayerControlPawn, DrawedRoomTile);
	DOREPLIFETIME(APlayerControlPawn, bStillInMove);
	DOREPLIFETIME(APlayerControlPawn, bIsOutOfMove);
	DOREPLIFETIME(APlayerControlPawn, CurrentMovementInputState);
	DOREPLIFETIME(APlayerControlPawn, FollowTarget);
	DOREPLIFETIME(APlayerControlPawn, CameraFollowSpeed);
	DOREPLIFETIME(APlayerControlPawn, RifleTargets);
}