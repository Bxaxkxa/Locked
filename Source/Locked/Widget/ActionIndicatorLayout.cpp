// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionIndicatorLayout.h"

void UActionIndicatorLayout::ChangeIndicatorLayout(EActionLayout ActionLayoutStyle)
{
	LayoutSwitcher->SetActiveWidgetIndex((int)ActionLayoutStyle);
}
