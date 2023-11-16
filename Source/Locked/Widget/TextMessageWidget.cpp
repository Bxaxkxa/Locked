// Fill out your copyright notice in the Description page of Project Settings.


#include "TextMessageWidget.h"
#include "Components/TextBlock.h"

void UTextMessageWidget::NativeConstruct()
{
	Super::NativeConstruct();

	OnVisibilityChanged.AddDynamic(this, &UTextMessageWidget::VisibilityChange);
}

void UTextMessageWidget::PlayFadeOutAnimation()
{
	PlayAnimation(FadeOutAnimation);
}

void UTextMessageWidget::VisibilityChange(ESlateVisibility InVisibility)
{
	if (InVisibility == ESlateVisibility::HitTestInvisible)
		PlayFadeOutAnimation();
}

void UTextMessageWidget::SetTextMessage(FString NewMessage)
{
	TextMessageBlock->SetText(FText::FromString(NewMessage));
}
