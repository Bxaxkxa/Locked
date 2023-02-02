// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyProjectGameMode.h"
#include "UObject/ConstructorHelpers.h"
#include "MyProject/Character/BoardController.h"
#include "MyProject/Character/State/LockedPlayerState.h"
#include "MyProject/SaveFile/BoardSettingsSave.h"
#include "MyProject/Widget/PlayerTurnDisplay.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameSession.h"
#include "Net/UnrealNetwork.h"
#include "UObject/ConstructorHelpers.h"
#include "LockedNavigationInputConfig.h"
#include "Blueprint/WidgetLayoutLibrary.h"

AMyProjectGameMode::AMyProjectGameMode()
{
	// set default pawn class to our Blueprinted character
	/*static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}*/

	SetReplicates(true);
}

void AMyProjectGameMode::StartPlay()
{
	Super::StartPlay();

	FSlateApplication::Get().SetNavigationConfig(MakeShared<FGameNavigationConfig>());

	UWidgetLayoutLibrary::RemoveAllWidgets(GetWorld());

	UBoardSettingsSave* GameSettingsSaveFile = Cast<UBoardSettingsSave>(UGameplayStatics::LoadGameFromSlot("BoardGameSettings", 0));
	BoardGameSettings = GameSettingsSaveFile->GetGameSettingsSave();

	AddTurnDisplay(0);
	NextPlayerTurn();

	DuelData.Add(FDuelData(true));
	DuelData.Add(FDuelData(false));
}

void AMyProjectGameMode::ShowNextTurnDisplay()
{
	AddTurnDisplay(CurrentPlayerTurn);
}

void AMyProjectGameMode::AddTurnDisplay(int PlayerTurnIndex)
{
	for (int i = 0; i < GetNumPlayers(); i++)
	{
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), i);

		ABoardController* BoardController = Cast<ABoardController>(PlayerController);
		if (BoardController)
		{
			BoardController->Client_DisplayPlayerTurn(PlayerTurnIndex);

			APlayerController* CurrentTurnPlayerController = UGameplayStatics::GetPlayerController(GetWorld(), PlayerTurnIndex);

			ABoardController* CurrentTurnBoardController = Cast<ABoardController>(CurrentTurnPlayerController);

			BoardController->Server_ChangeCameraPerspective(CurrentTurnBoardController->GetPlayerCharacter());
		}
	}
}

void AMyProjectGameMode::DuelSetup(ABoardController* Attacker, ABoardController* Defender)
{
	DuelAttacker = Attacker;
	DuelDefender = Defender;
}

void AMyProjectGameMode::SwitchDuelState()
{
	DuelDefender->Client_ChangeDuelState();
}

void AMyProjectGameMode::QueuUpWeapon(FDuelData QueuDuelData)
{
	int32 DataIndex = DuelData.Find(QueuDuelData);
	DuelData[DataIndex] = QueuDuelData;

	DuelAttacker->Client_UpdateDuelUI(DuelData);
	DuelDefender->Client_UpdateDuelUI(DuelData);

	GetWorld()->GetTimerManager().SetTimer(DuelStartDelayTimerHandle, this, &AMyProjectGameMode::CheckDuelCondition, DuelStartDelayTime, false);
	//CheckDuelCondition();
}

void AMyProjectGameMode::UnQueuUpWeapon(bool IsPlayerAttacker)
{
	int32 DataIndex = DuelData.Find(IsPlayerAttacker);
	DuelData[DataIndex].IsReady = false;
}

void AMyProjectGameMode::CheckDuelCondition()
{
	FDuelData* AttackerDuelData = DuelData.FindByKey(true);
	FDuelData* DefenderDuelData = DuelData.FindByKey(false);

	if (AttackerDuelData->IsReady && DefenderDuelData->IsReady)
	{
		EDuelResult CalculatedDuelResult = EDuelResult::AttackerWin;
		if (AttackerDuelData->WeaponValue == DefenderDuelData->WeaponValue)
		{
			CalculatedDuelResult = EDuelResult::Draw;
		}
		else if (AttackerDuelData->WeaponValue < DefenderDuelData->WeaponValue)
		{
			CalculatedDuelResult = EDuelResult::DefenderWin;
		}

		//Remove item from inventory if it is not Fist
		if (AttackerDuelData->WeaponValue)
			DuelAttacker->Server_UsedUpWeapon(AttackerDuelData->ItemData);
		if (DefenderDuelData->WeaponValue)
			DuelDefender->Server_UsedUpWeapon(DefenderDuelData->ItemData);

		DuelAttacker->Client_PlayDuelAnimation(CalculatedDuelResult);
		DuelDefender->Client_PlayDuelAnimation(CalculatedDuelResult);
	}
}

void AMyProjectGameMode::StopDuel(bool RecheckDuelTarget)
{
	DuelAttacker->Client_HideDuelOption();
	DuelDefender->Client_HideDuelOption();

	DuelAttacker->Client_HideDuelUI();
	DuelDefender->Client_HideDuelUI();

	DuelAttacker->Client_ShowIndicatorLayout(false);
	DuelDefender->Client_ShowIndicatorLayout(false);

	if (RecheckDuelTarget)
	{
		DuelAttacker->CheckRoomForDualTarget();
		return;
	}

	DuelAttacker->Server_EndTurn();
}

void AMyProjectGameMode::StartDuel()
{
	DuelAttacker->Client_HideDuelOption();
	DuelDefender->Client_HideDuelOption();

	DuelAttacker->Client_DisplayDuelUI();
	DuelDefender->Client_DisplayDuelUI();
}

void AMyProjectGameMode::UpdateAttackerDuelDiceData(bool IsStillRolling, int32 DiceNumber)
{
	DuelDiceData.IsAttackerStillRolling = IsStillRolling;
	DuelDiceData.AttackerDiceNumber = DiceNumber;
	UpdateDuelDiceUI();
}

void AMyProjectGameMode::UpdateDefenderDuelDiceData(bool IsStillRolling, int32 DiceNumber)
{
	DuelDiceData.IsDefenderStillRolling = IsStillRolling;
	DuelDiceData.DefenderDiceNumber = DiceNumber;
	UpdateDuelDiceUI();
}

void AMyProjectGameMode::UpdateDuelDiceUI()
{
	DuelAttacker->Client_UpdateDuelDiceVisual(DuelDiceData);
	DuelDefender->Client_UpdateDuelDiceVisual(DuelDiceData);

	if (!DuelDiceData.IsAttackerStillRolling && !DuelDiceData.IsDefenderStillRolling)
		GetWorld()->GetTimerManager().SetTimer(DuelStartDelayTimerHandle, this, &AMyProjectGameMode::CheckDuelDiceResult, DuelStartDelayTime, false);
}

void AMyProjectGameMode::CheckDuelDiceResult()
{
	EDuelResult CalculatedDuelResult = EDuelResult::AttackerWin;
	if (DuelDiceData.AttackerDiceNumber == DuelDiceData.DefenderDiceNumber)
	{
		CalculatedDuelResult = EDuelResult::Draw;
	}
	else if (DuelDiceData.AttackerDiceNumber < DuelDiceData.DefenderDiceNumber)
	{
		CalculatedDuelResult = EDuelResult::DefenderWin;
	}

	DuelAttacker->Server_DuelDiceOutcome(CalculatedDuelResult);
	DuelDefender->Server_DuelDiceOutcome(CalculatedDuelResult);

}
void AMyProjectGameMode::UpdatePlayerHealthInfo()
{
	UWorld* World = GetWorld();

	TArray<int> PlayersHealth;
	for (int32 i = 0; i < GetNumPlayers(); i++)
	{
		APlayerState* PlayerState = UGameplayStatics::GetPlayerState(World, i);
		ALockedPlayerState* LockedPlayerState = Cast<ALockedPlayerState>(PlayerState);

		PlayersHealth.Add(LockedPlayerState->GetHealthPoint());
	}

	for (int32 i = 0; i < GetNumPlayers(); i++)
	{
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), i);
		ABoardController* BoardContoller = Cast<ABoardController>(PlayerController);

		BoardContoller->Client_UpdatePlayerHealthInfo(PlayersHealth);
	}
}


void AMyProjectGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (NewPlayer->IsLocalPlayerController())
	{
		UBoardSettingsSave* GameSettingsSaveFile = Cast<UBoardSettingsSave>(UGameplayStatics::LoadGameFromSlot("BoardGameSettings", 0));
		BoardGameSettings = GameSettingsSaveFile->GetGameSettingsSave();
	}
}

void AMyProjectGameMode::NextPlayerTurn()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), CurrentPlayerTurn);
	ABoardController* BoardContoller = Cast<ABoardController>(PlayerController);
	if (BoardContoller)
	{
		BoardContoller->Server_StartPlayerTurn();
	}

	CurrentPlayerTurn = (CurrentPlayerTurn + 1) % GetNumPlayers();
}

void AMyProjectGameMode::Server_StartInitialTurn_Implementation()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	ABoardController* BoardContoller = Cast<ABoardController>(PlayerController);
	if (BoardContoller)
	{
		BoardContoller->Server_StartPlayerTurn();
	}
}

void AMyProjectGameMode::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMyProjectGameMode, CurrentPlayerTurn);
	DOREPLIFETIME(AMyProjectGameMode, NumberOfplayer);
	DOREPLIFETIME(AMyProjectGameMode, DuelAttacker);
	DOREPLIFETIME(AMyProjectGameMode, DuelDefender);
	DOREPLIFETIME(AMyProjectGameMode, DuelData);
	DOREPLIFETIME(AMyProjectGameMode, DuelDiceData);
}
