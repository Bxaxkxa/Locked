// Fill out your copyright notice in the Description page of Project Settings.


#include "DuelOption.h"
#include "SelectedButton.h"
#include "Components/HorizontalBox.h"
#include "MyProject/Character/BoardController.h"
#include "MyProject/Character/PlayerControlPawn.h"
#include "MyProject/GameMode/MyProjectGameMode.h"
#include "Net/UnrealNetwork.h"

void UDuelOption::NativeConstruct()
{
	Super::NativeConstruct();

	if (ChallengeButton->OnButtonClick.IsBound())
		return;

	ChallengeButton->OnButtonClick.AddDynamic(this, &UDuelOption::ChallegeDuel);
	IgnoreButton->OnButtonClick.AddDynamic(this, &UDuelOption::IgnoreDuel);

	//OnVisibilityChanged.AddDynamic(this, &UDuelOption::VisibilityChange);
}

void UDuelOption::IgnoreDuel()
{
	ABoardController* PlayerController = GetOwningPlayer<ABoardController>();

	if (PlayerController->IsPlayerTheDuelAttacker())
	{
		PlayAnimation(ToWaitingFadeAnimation);
		PlayerController->Server_TellGameModeToUpdateDuelState();

		return;
	}

	PlayerController->Server_StopDuel();
}

void UDuelOption::ChallegeDuel()
{
	ABoardController* PlayerController = GetOwningPlayer<ABoardController>();

	PlayerController->Server_StartDuel();
}

void UDuelOption::PlayToOptionAnimation()
{
	PlayAnimation(ToOptionFadeAnimation);
}

void UDuelOption::VisibilityChange(ESlateVisibility InVisibility)
{
	ABoardController* PlayerOwner = GetOwningPlayer<ABoardController>();

	if (InVisibility == ESlateVisibility::SelfHitTestInvisible)
	{
		PlayAnimation(InitialFadeInAnimation);
		if (PlayerOwner->IsPlayerTheDuelAttacker())
		{
			ChallengeRemainingBox->SetVisibility(ESlateVisibility::HitTestInvisible);
			//ChallengeButton->SetIsEnabled((bool)PlayerOwner->GetRemainingChallegeTimes());

			//(bool)OwnerController->GetRemainingChallegeTimes() ? ChallengeButton->SetFocus() : IgnoreButton->SetFocus();

			return;
		}
		ChallengeRemainingBox->SetVisibility(ESlateVisibility::Hidden);

		return;
	}
}

void UDuelOption::SetDuelTarget(APlayerControlPawn* NewDuelTarget)
{
	DuelTarget = NewDuelTarget;
}

void UDuelOption::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UDuelOption, DuelTarget);
}
