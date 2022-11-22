// Fill out your copyright notice in the Description page of Project Settings.


#include "BoardController.h"
#include "LockedCharacter.h"
#include "PlayerControlPawn.h"
#include "MyProject/GameMode/MyProjectGameMode.h"
#include "MyProject/Widget/PlayerActionWidget.h"
#include "MyProject/Widget/PlayerTurnDisplay.h"
#include "MyProject/Widget/ActionIndicatorLayout.h"
#include "State/LockedPlayerState.h"
#include "Net/UnrealNetwork.h"

void ABoardController::BeginPlay()
{
	Super::BeginPlay();

	SetInputMode(FInputModeGameAndUI());

	ALockedPlayerState* State = GetPlayerState<ALockedPlayerState>();
	if (State) 
	{
		State->OnMovementBehaviourChangeDelegate.BindUFunction(this, FName("ChangeCameraBehaviour"));
	}

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

		InputComponent->BindAction("PlaceTile", IE_Pressed, this, &ABoardController::Server_PlaceRoomTile);

		InputComponent->BindAction("DrawTile", IE_Pressed, this, &ABoardController::Server_DrawRoomTile);
		InputComponent->BindAction("RotateTile", IE_Pressed, this, &ABoardController::Server_RotateRoomTile);
		InputComponent->BindAction("Back", IE_Pressed, this, &ABoardController::Back);
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

void ABoardController::Client_ShowActionWidget_Implementation(bool On)
{
	if (ActionUIWidget)
	{
		On ? ActionUIWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible) : ActionUIWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void ABoardController::ShowActionWidget(bool On)
{
	//Start drag quick time event after fade out finish
	FTimerDelegate TimerDel;
	TimerDel.BindUFunction(this, FName("Client_ShowActionWidget"), On);

	GetWorld()->GetTimerManager().SetTimer(UIDelayTimerHandle, TimerDel, UIDelayTime, false);

	//if (ActionUIWidget)
	//{
	//	On ? ActionUIWidget->AddToViewport() : ActionUIWidget->RemoveFromParent();
	//}
}

void ABoardController::Client_ShowIndicatorLayout_Implementation(bool On)
{
	if (ActionIndicatorWidget)
	{
		On ? ActionIndicatorWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible) : ActionIndicatorWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void ABoardController::Client_ChangeIndicatorLayout_Implementation(EActionLayout LayoutStyle)
{
	if (ActionIndicatorWidget)
	{
		if (!ActionIndicatorWidget->IsVisible())
		{
			ActionIndicatorWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
	ActionIndicatorWidget->ChangeIndicatorLayout(LayoutStyle);
	}
}

void ABoardController::Back()
{
	if (BoardPlayer)
	{
		BoardPlayer->BackToActionMenu();
	}
}

void ABoardController::Server_ChangeCameraBehaviour_Implementation(EMovementInputState NewInputState)
{
	if (BoardPlayer && bIsInTurn)
	{
		BoardPlayer->ChangeCameraBehaviour(NewInputState);
	}
}

void ABoardController::Client_DisplayPlayerTurn_Implementation(int Turn)
{
	if (TurnDisplayWidgetClass)
	{
		UPlayerTurnDisplay* TurnDisplay = Cast<UPlayerTurnDisplay>(CreateWidget(this, TurnDisplayWidgetClass));
		if (TurnDisplay)
		{
			TurnDisplay->AddToViewport();
			TurnDisplay->PlayShowTurnAnimation(Turn);
		}
	}
}

void ABoardController::Multicast_StartPlayerTurn_Implementation()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		bIsInTurn = true;

		BoardPlayer->StartTurn();
	}

	/*if (IsLocalPlayerController())
	{
		ShowActionWidget(true);
	}*/
}

void ABoardController::Server_ChangeCameraPerspective_Implementation(ALockedCharacter* CurrentTurnPlayerCharacter)
{
	if (BoardPlayer)
	{
		BoardPlayer->ChangeCameraPerspective(CurrentTurnPlayerCharacter);
	}
}

void ABoardController::Multicast_EndTurn_Implementation()
{
	Server_ChangeTurn();

	if (IsLocalPlayerController())
	{
		ShowActionWidget(false);
	}
}

void ABoardController::Server_ChangeTurn_Implementation()
{
	if (bIsInTurn)
	{
		bIsInTurn = false;

		BoardPlayer->EndTurn();

		AMyProjectGameMode* GameMode = (AMyProjectGameMode*)GetWorld()->GetAuthGameMode();
		GameMode->ShowNextTurnDisplay();
		GameMode->NextPlayerTurn();
	}
}

void ABoardController::Server_DrawRoomTile_Implementation()
{
	if (BoardPlayer && bIsInTurn)
	{
		BoardPlayer->DrawRoomTile();
		Client_ChangeIndicatorLayout(EActionLayout::PlaceTileAction);
	}
}

void ABoardController::Server_PlaceRoomTile_Implementation()
{
	if (BoardPlayer && bIsInTurn)
	{
		BoardPlayer->PlaceRoomTile();
		Client_ChangeIndicatorLayout(EActionLayout::MoveAction);
	}
}

void ABoardController::Server_RotateRoomTile_Implementation()
{
	if (BoardPlayer && bIsInTurn)
	{
		BoardPlayer->RotateRoomTile();
	}
}

ALockedCharacter* ABoardController::GetPlayerCharacter()
{
	return BoardPlayer->GetPlayerCharacter();
}

void ABoardController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABoardController, BoardPlayer);
	DOREPLIFETIME(ABoardController, bIsInTurn);
	DOREPLIFETIME(ABoardController, ActionUIWidget);
	DOREPLIFETIME(ABoardController, ActionIndicatorWidget);
	DOREPLIFETIME(ABoardController, TurnDisplayWidgetClass);
	DOREPLIFETIME(ABoardController, bIsReadyToStart);
}
