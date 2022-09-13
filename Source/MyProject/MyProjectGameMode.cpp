// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyProjectGameMode.h"
#include "UObject/ConstructorHelpers.h"
#include "MyProject/Character/BoardController.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

AMyProjectGameMode::AMyProjectGameMode()
{
	// set default pawn class to our Blueprinted character
	/*static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}*/
}

void AMyProjectGameMode::StartPlay()
{
	Super::StartPlay();

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	ABoardController* BoardContoller = Cast<ABoardController>(PlayerController);
	BoardContoller->StartPlayerTurn();
}

void AMyProjectGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	NumberOfplayer++;
}

void AMyProjectGameMode::NextPlayerTurn()
{
	CurrentPlayerTurn = (CurrentPlayerTurn + 1) % NumberOfplayer;

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), CurrentPlayerTurn);
	ABoardController* BoardContoller = Cast<ABoardController>(PlayerController);
	BoardContoller->StartPlayerTurn();
}

void AMyProjectGameMode::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMyProjectGameMode, CurrentPlayerTurn);
	DOREPLIFETIME(AMyProjectGameMode, NumberOfplayer);
}