// Fill out your copyright notice in the Description page of Project Settings.


#include "BoardController.h"
#include "LockedCharacter.h"
#include "PlayerControlPawn.h"
#include "MyProject/MyProjectGameMode.h"
#include "Net/UnrealNetwork.h"

void ABoardController::BeginPlay()
{
	Super::BeginPlay();

	//SetInputMode(FInputModeUIOnly());

	SetReplicates(true);
}

void ABoardController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (InputComponent)
	{
		InputComponent->BindAction<FTestingDelegate, ABoardController, ETileDirection>("Left", IE_Pressed, this, &ABoardController::Server_ManageMovement, ETileDirection::NextLeft);
		InputComponent->BindAction<FTestingDelegate, ABoardController, ETileDirection>("Right", IE_Pressed, this, &ABoardController::Server_ManageMovement, ETileDirection::NextRight);
		InputComponent->BindAction<FTestingDelegate, ABoardController, ETileDirection>("Up", IE_Pressed, this, &ABoardController::Server_ManageMovement, ETileDirection::NextUp);
		InputComponent->BindAction<FTestingDelegate, ABoardController, ETileDirection>("Down", IE_Pressed, this, &ABoardController::Server_ManageMovement, ETileDirection::NextDown);

		InputComponent->BindAction("ChangeBehaviour", IE_Pressed, this, &ABoardController::ChangeCameraBehaviour);

		InputComponent->BindAction("PlaceTile", IE_Pressed, this, &ABoardController::Server_PlaceRoomTile);

		InputComponent->BindAction("DrawTile", IE_Pressed, this, &ABoardController::Server_DrawRoomTile);
		InputComponent->BindAction("RotateTile", IE_Pressed, this, &ABoardController::Server_RotateRoomTile);
		InputComponent->BindAction("EndTurn", IE_Pressed, this, &ABoardController::Server_EndTurn);
	}
}

void ABoardController::AcknowledgePossession(APawn* PossesedPawn)
{
	Super::AcknowledgePossession(PossesedPawn);
}

ABoardController::ABoardController()
{
}

void ABoardController::Tick(float DeltaTime)
{
}

void ABoardController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	BoardPlayer = Cast<APlayerControlPawn>(aPawn);
}

void ABoardController::OnUnPossess()
{
	Super::OnUnPossess();
}

void ABoardController::Server_ManageMovement_Implementation(ETileDirection MoveToDirection)
{
	if (BoardPlayer && bIsInTurn)
	{
		BoardPlayer->ManageMovement(MoveToDirection);
	}
}

void ABoardController::ChangeCameraBehaviour()
{
	if (BoardPlayer && bIsInTurn)
	{
		BoardPlayer->ChangeCameraBehaviour();
	}
}

void ABoardController::StartPlayerTurn()
{
	bIsInTurn = true;
}

void ABoardController::Server_EndTurn_Implementation()
{
	if (bIsInTurn)
	{
		bIsInTurn = false;

		AMyProjectGameMode* GameMode = (AMyProjectGameMode*)GetWorld()->GetAuthGameMode();
		GameMode->NextPlayerTurn();
	}
}

void ABoardController::Server_DrawRoomTile_Implementation()
{
	if (BoardPlayer && bIsInTurn)
	{
		BoardPlayer->DrawRoomTile();
	}
}

void ABoardController::Server_PlaceRoomTile_Implementation()
{
	if (BoardPlayer && bIsInTurn)
	{
		BoardPlayer->PlaceRoomTile();
	}
}

void ABoardController::Server_RotateRoomTile_Implementation()
{
	if (BoardPlayer && bIsInTurn)
	{
		BoardPlayer->RotateRoomTile();
	}
}

void ABoardController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABoardController, BoardPlayer);
	DOREPLIFETIME(ABoardController, bIsInTurn);
}