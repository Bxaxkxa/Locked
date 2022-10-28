// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerTurnDisplay.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UPlayerTurnDisplay : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* ShowTurn;
	UPROPERTY(BlueprintReadOnly)
		int CurrentPlayerTurn;
	
	UFUNCTION(Server,Reliable)
		virtual void Server_StartPlayerTurn();

	void PlayShowTurnAnimation(int PlayerTurn);

	virtual void NativeConstruct() override;
};
