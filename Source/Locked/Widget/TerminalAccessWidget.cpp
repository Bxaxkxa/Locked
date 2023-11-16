// Fill out your copyright notice in the Description page of Project Settings.


#include "TerminalAccessWidget.h"
#include "Components/WidgetSwitcher.h"

void UTerminalAccessWidget::ShowAccessAnimation(bool bIsOn, bool bIsServer)
{
	AccessIndex->SetActiveWidgetIndex(bIsServer);

	//PlayAnimation(ShowAccess, GetAnimationCurrentTime(ShowAccess), 1, bIsOn ? EUMGSequencePlayMode::Forward : EUMGSequencePlayMode::Reverse);
	PlayAnimation(ShowAccess, 0.1, 1, bIsOn ? EUMGSequencePlayMode::Forward : EUMGSequencePlayMode::Reverse);
}
