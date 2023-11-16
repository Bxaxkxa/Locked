// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyProjectGameMode.h"
#include "UObject/ConstructorHelpers.h"
#include "Locked/Character/BoardController.h"
#include "Locked/Character/State/LockedPlayerState.h"
#include "Locked/Room/RoomTile.h"
#include "Locked/Room/RandomGenRoom.h"
#include "Locked/Components/InventoryComponent.h"
#include "Locked/SaveFile/BoardSettingsSave.h"
#include "Locked/Widget/PlayerTurnDisplay.h"
#include "Locked/Widget/LoadingScreen.h"
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
	PrimaryActorTick.bCanEverTick = true;

	SetReplicates(true);
}

void AMyProjectGameMode::StartPlay()
{
	Super::StartPlay();

	FSlateApplication::Get().SetNavigationConfig(MakeShared<FGameNavigationConfig>());

	UWorld* World = GetWorld();

	//TODO: For Debug
	FTimerHandle TempTimer = FTimerHandle();
	if (bIsInDebug)
		World->GetTimerManager().SetTimer(TempTimer, this, &AMyProjectGameMode::Server_StartInitialTurn, 0.2f, false);
	//Server_StartInitialTurn();
	UBoardSettingsSave* GameSettingsSaveFile = Cast<UBoardSettingsSave>(UGameplayStatics::LoadGameFromSlot("BoardGameSettings", 0));
	if (!GameSettingsSaveFile)
	{
		GameSettingsSaveFile = Cast<UBoardSettingsSave>(UGameplayStatics::CreateSaveGameObject(UBoardSettingsSave::StaticClass()));

		UGameplayStatics::SaveGameToSlot(GameSettingsSaveFile, "BoardGameSettings", 0);
	}

	BoardGameSettings = GameSettingsSaveFile->GetGameSettingsSave();

	AActor* OutActor = UGameplayStatics::GetActorOfClass(World, ARandomGenRoom::StaticClass());
	if (OutActor)
	{
		Cast<ARandomGenRoom>(OutActor)->GenerateTile(GameSettingsSaveFile);
	}


	DuelData.Add(FDuelData(true));
	DuelData.Add(FDuelData(false));
}

void AMyProjectGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Duel Test (not done yet)
	//FString IsAttackerString = (DuelData[0].IsAttacker ? "True" : "False");
	//FString IsReadyString = (DuelData[0].IsReady ? "True" : "False");

	//GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green, "IsAttacker? " + IsAttackerString);
	//GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green, "IsReady? " + IsReadyString);
	//GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green, "ItemName? " + UEnum::GetValueAsString(DuelData[0].ItemData.ItemName));

	//IsAttackerString = (DuelData[1].IsAttacker ? "True" : "False");
	//IsReadyString = (DuelData[1].IsReady ? "True" : "False");

	//GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, "IsAttacker? " + IsAttackerString);
	//GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, "IsReady? " + IsReadyString);
	//GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, "ItemName? " + UEnum::GetValueAsString(DuelData[1].ItemData.ItemName));

	//GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, "CurrentPendingResult? " + UEnum::GetValueAsString(PendingDuelResult));
}

void AMyProjectGameMode::HandleSeamlessTravelPlayer(AController*& C)
{
	Super::HandleSeamlessTravelPlayer(C);

	PlayerFinishedTravelCount++;

	if (PlayerFinishedTravelCount == GetNumPlayers())
	{
		GetWorld()->GetTimerManager().SetTimer(GameStartDelayTimerHandle, this, &AMyProjectGameMode::RemoveLoadingScreen, GameStartDelayTime, false);
	}
}

void AMyProjectGameMode::RemoveLoadingScreen()
{
	for (int i = 0; i < GetNumPlayers(); i++)
	{
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), i);

		ABoardController* BoardController = Cast<ABoardController>(PlayerController);
		if (BoardController)
		{
			BoardController->Client_RemoveLoadingScreen();
		}
	}
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
}

void AMyProjectGameMode::UnQueuUpWeapon(bool IsPlayerAttacker)
{
	int32 DataIndex = DuelData.Find(IsPlayerAttacker);
	DuelData[DataIndex].IsReady = false;

	DuelAttacker->Client_UpdateDuelUI(DuelData);
	DuelDefender->Client_UpdateDuelUI(DuelData);

	GetWorld()->GetTimerManager().ClearTimer(DuelStartDelayTimerHandle);
}

void AMyProjectGameMode::RefreshDuelData()
{
	DuelData.FindByKey(true)->Refresh();
	DuelData.FindByKey(false)->Refresh();
}

void AMyProjectGameMode::CheckDuelCondition()
{
	FDuelData* AttackerDuelData = DuelData.FindByKey(true);
	FDuelData* DefenderDuelData = DuelData.FindByKey(false);

	if (AttackerDuelData->IsReady && DefenderDuelData->IsReady)
	{
		PendingDuelResult = EDuelResult::AttackerWin;
		if (AttackerDuelData->WeaponValue == DefenderDuelData->WeaponValue)
		{
			PendingDuelResult = EDuelResult::Draw;
		}
		else if (AttackerDuelData->WeaponValue < DefenderDuelData->WeaponValue)
		{
			PendingDuelResult = EDuelResult::DefenderWin;
		}

		//Remove item from inventory if it is not Fist
		if (AttackerDuelData->WeaponValue)
			DuelAttacker->Server_UsedUpWeapon(AttackerDuelData->ItemData);
		if (DefenderDuelData->WeaponValue)
			DuelDefender->Server_UsedUpWeapon(DefenderDuelData->ItemData);

		DuelAttacker->Client_PlayDuelAnimation(PendingDuelResult);
		DuelDefender->Client_PlayDuelAnimation(PendingDuelResult);
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
	PendingDuelResult = EDuelResult::AttackerWin;
	if (DuelDiceData.AttackerDiceNumber == DuelDiceData.DefenderDiceNumber)
	{
		PendingDuelResult = EDuelResult::Draw;
	}
	else if (DuelDiceData.AttackerDiceNumber < DuelDiceData.DefenderDiceNumber)
	{
		PendingDuelResult = EDuelResult::DefenderWin;
	}

	DuelAttacker->Client_HideDuelUI();
	DuelDefender->Client_HideDuelUI();

	StartPlayerStealCard(PendingDuelResult);
}

void AMyProjectGameMode::StartPlayerStealCard(EDuelResult DuelResult)
{
	TArray<FItemData> StealedInventory;
	switch (DuelResult)
	{
	case EDuelResult::AttackerWin:
		StealedInventory = DuelDefender->GetInventory()->ItemInventory;

		if (!StealedInventory.Num())
			RunPendingDuelResult();

		DuelAttacker->Client_StealOpponentInventory(StealedInventory);
		break;
	case EDuelResult::DefenderWin:
		StealedInventory = DuelAttacker->GetInventory()->ItemInventory;

		if (!StealedInventory.Num())
			RunPendingDuelResult();

		DuelDefender->Client_StealOpponentInventory(StealedInventory);
		break;
	case EDuelResult::Draw:
		RunPendingDuelResult();
		break;
	}
}

void AMyProjectGameMode::StealOpponentCard(FItemData StolenItem)
{
	switch (PendingDuelResult)
	{
	case EDuelResult::AttackerWin:
		DuelDefender->GetInventory()->Server_RemoveItemFromInventory(StolenItem);
		break;
	case EDuelResult::DefenderWin:
		DuelAttacker->GetInventory()->Server_RemoveItemFromInventory(StolenItem);
		break;
	}
}

void AMyProjectGameMode::RunPendingDuelResult()
{
	RefreshDuelData();

	switch (PendingDuelResult)
	{
	case EDuelResult::AttackerWin:
		DuelDefender->TakeDamage(1.0f, FDamageEvent(), nullptr, DuelAttacker);
		StopDuel(true);
		break;
	case EDuelResult::DefenderWin:
		DuelAttacker->TakeDamage(1.0f, FDamageEvent(), nullptr, DuelDefender);
		StopDuel(false);
		break;
	case EDuelResult::Draw:
		StopDuel(true);
		break;
	default:
		break;
	}
}

void AMyProjectGameMode::SetPlayerHealthInfo()
{
	UWorld* World = GetWorld();

	TArray<FString> PlayersName;
	for (int32 i = 0; i < GetNumPlayers(); i++)
	{
		APlayerState* PlayerState = UGameplayStatics::GetPlayerState(World, i);

		PlayersName.Add(PlayerState->GetPlayerName());
	}

	for (int32 i = 0; i < GetNumPlayers(); i++)
	{
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), i);
		ABoardController* BoardContoller = Cast<ABoardController>(PlayerController);

		BoardContoller->Client_SetPlayerHealthInfo(PlayersName);
	}
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
		ABoardController* BoardController = Cast<ABoardController>(PlayerController);

		BoardController->Client_UpdatePlayerHealthInfo(PlayersHealth);
	}
}

void AMyProjectGameMode::UpdatePlayerInventoryInfo()
{
	TArray<int> PlayersItemCount;
	TArray<int> PlayersTreasureCount;
	for (int32 i = 0; i < GetNumPlayers(); i++)
	{
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), i);
		ABoardController* BoardController = Cast<ABoardController>(PlayerController);
		TArray<FItemData> PlayerInventory = BoardController->GetInventory()->ItemInventory;

		PlayersItemCount.Add(PlayerInventory.Num());
		PlayersTreasureCount.Add(CalculatePlayerUpdatedTreasure(PlayerInventory));
	}

	for (int32 i = 0; i < GetNumPlayers(); i++)
	{
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), i);
		ABoardController* BoardController = Cast<ABoardController>(PlayerController);

		BoardController->Client_UpdatePlayerInventoryInfo(PlayersItemCount, PlayersTreasureCount);
	}
}

int AMyProjectGameMode::CalculatePlayerUpdatedTreasure(TArray<FItemData> PlayerInventory)
{
	int TreasureCount = 0;
	for (FItemData it : PlayerInventory)
	{
		TreasureCount += it.bIsTreasure;
	}

	return TreasureCount;
}


void AMyProjectGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	ABoardController* BoardController = Cast<ABoardController>(NewPlayer);
	if (BoardController)
	{
		FTimerHandle TimerHandle = FTimerHandle();

		GetWorld()->GetTimerManager().SetTimer(TimerHandle, BoardController, &ABoardController::Client_AddAllGUIToViewport, 0.2f, false);
		//BoardController->Client_AddAllGUIToViewport();
	}
	//Depricated
	//if (NewPlayer->IsLocalPlayerController())
	//{
	//	UBoardSettingsSave* GameSettingsSaveFile = Cast<UBoardSettingsSave>(UGameplayStatics::LoadGameFromSlot("BoardGameSettings", 0));
	//	if (!GameSettingsSaveFile)
	//	{
	//		GameSettingsSaveFile = Cast<UBoardSettingsSave>(UGameplayStatics::CreateSaveGameObject(UBoardSettingsSave::StaticClass()));

	//		//GameSettingsSaveFile->SetGameSettingsSave(BoardGameSettings);
	//		UGameplayStatics::SaveGameToSlot(GameSettingsSaveFile, "BoardGameSettings", 0);
	//	}
	//	BoardGameSettings = GameSettingsSaveFile->GetGameSettingsSave();
	//}
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

	if (CurrentPlayerTurn == 0)
	{
		TArray<AActor*> AllRoomActor;

		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARoomTile::StaticClass(), AllRoomActor);

		for (AActor* it : AllRoomActor)
		{
			ARoomTile* RoomTile = Cast<ARoomTile>(it);
			RoomTile->Server_UpdateRoomState();
		}
	}
}

void AMyProjectGameMode::Server_StartInitialTurn_Implementation()
{
	AddTurnDisplay(0);
	NextPlayerTurn();

	//CurrentPlayerTurn = (CurrentPlayerTurn + 1) % GetNumPlayers();

	//TODO: Use for Packaged version
	if (!bIsInDebug)
	{
		for (int i = 0; i < GetNumPlayers(); i++)
		{
			APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), i);

			ABoardController* BoardController = Cast<ABoardController>(PlayerController);
			if (BoardController)
			{
				BoardController->Client_AddAllGUIToViewport();
			}
		}
	}
}

void AMyProjectGameMode::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMyProjectGameMode, CurrentPlayerTurn);
	DOREPLIFETIME(AMyProjectGameMode, NumberOfPlayer);
	DOREPLIFETIME(AMyProjectGameMode, DuelAttacker);
	DOREPLIFETIME(AMyProjectGameMode, DuelDefender);
	DOREPLIFETIME(AMyProjectGameMode, DuelData);
	DOREPLIFETIME(AMyProjectGameMode, DuelDiceData);
}
