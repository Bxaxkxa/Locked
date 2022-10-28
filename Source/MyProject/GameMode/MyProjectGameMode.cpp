// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyProjectGameMode.h"
#include "UObject/ConstructorHelpers.h"
#include "MyProject/Character/BoardController.h"
#include "MyProject/SaveFile/BoardSettingsSave.h"
#include "MyProject/Widget/PlayerTurnDisplay.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameSession.h"
#include "Net/UnrealNetwork.h"

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

		Cast<ABoardController>(PlayerController)->Client_DisplayPlayerTurn(PlayerTurnIndex);
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
		BoardContoller->Multicast_StartPlayerTurn();
	}

	CurrentPlayerTurn = (CurrentPlayerTurn + 1) % GetNumPlayers();
}

void AMyProjectGameMode::Server_StartInitialTurn_Implementation()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	ABoardController* BoardContoller = Cast<ABoardController>(PlayerController);
	if (BoardContoller)
	{
		BoardContoller->Multicast_StartPlayerTurn();
	}
}

void AMyProjectGameMode::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMyProjectGameMode, CurrentPlayerTurn);
	DOREPLIFETIME(AMyProjectGameMode, NumberOfplayer);
}
