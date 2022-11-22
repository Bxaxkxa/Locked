// Fill out your copyright notice in the Description page of Project Settings.


#include "LockedNavigationInputConfig.h"
#include "MyProject/SaveFile/InputSaveFile.h"
#include "Kismet/GameplayStatics.h"

EUINavigationAction FGameNavigationConfig::GetNavigationActionFromKey(const FKeyEvent& InKeyEvent) const
{
	UInputSaveFile* InputSettingSaveFile = Cast<UInputSaveFile>(UGameplayStatics::LoadGameFromSlot("GameInputSettings", 0));

	FKey InKey = InKeyEvent.GetKey();
	if (InKey == InputSettingSaveFile->KeyboardConfirmKey || InKey == InputSettingSaveFile->GamepadConfirmKey)
	{
		// By default, enter, space, and gamepad accept are all counted as accept
		return EUINavigationAction::Accept;
	}
	//else if (InKey == EKeys::Escape || InKey == EKeys::Virtual_Back)
	//{
	//	// By default, escape and gamepad back count as leaving current scope
	//	return EUINavigationAction::Back;
	//}

	return EUINavigationAction::Invalid;
}
