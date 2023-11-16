// Fill out your copyright notice in the Description page of Project Settings.


#include "ObtainedItemUI.h"

void UObtainedItemUI::ObtainItem(FItemData NewItemData, EObtainItemMethod ObtainMethod)
{
	ItemData = NewItemData;

	PendingObtainMethod = ObtainMethod;

	PlayAnimation(ObtainedItemAnimation);
}
