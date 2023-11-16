// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSettingTerminal.h"
#include "Locked/Character/LobbyCharacter.h"
#include "Locked/Widget/TerminalAccessWidget.h"
#include "Components/WidgetComponent.h"

// Sets default values
AGameSettingTerminal::AGameSettingTerminal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AccessWidget = CreateDefaultSubobject<UWidgetComponent>("AccessWidget");
	AccessWidget->SetWidgetSpace(EWidgetSpace::World);
	AccessWidget->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AGameSettingTerminal::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGameSettingTerminal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGameSettingTerminal::Interact(APawn* InteractionInstigator)
{
	if (!IsNetMode(ENetMode::NM_ListenServer))
	{
		return;
	}

	Cast<ALobbyCharacter>(InteractionInstigator)->OpenGameSettingMenu();
}

void AGameSettingTerminal::ShowInteractIndicator(bool bState)
{
	Super::ShowInteractIndicator(bState);

	ShowAccessState(bState);
}

void AGameSettingTerminal::ShowAccessState_Implementation(bool bIsOn)
{
	UUserWidget* WidgetObject = AccessWidget->GetUserWidgetObject();
	UTerminalAccessWidget* CountWidget = Cast<UTerminalAccessWidget>(WidgetObject);
	
	CountWidget->ShowAccessAnimation(bIsOn, GetWorld()->IsNetMode(ENetMode::NM_ListenServer));
}
