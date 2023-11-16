// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Locked/LobbyElement/LobbyInteractableBase.h"
#include "GameSettingTerminal.generated.h"

UCLASS()
class LOCKED_API AGameSettingTerminal : public ALobbyInteractableBase
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGameSettingTerminal();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Component")
		class UWidgetComponent* AccessWidget;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Interact(APawn* InteractionInstigator) override;

	virtual void ShowInteractIndicator(bool bState) override;

	UFUNCTION(NetMulticast, Reliable)
	//UFUNCTION(Client, Reliable)
		void ShowAccessState(bool bIsOn);
};
