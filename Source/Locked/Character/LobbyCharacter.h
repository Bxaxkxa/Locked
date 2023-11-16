// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "LobbyCharacter.generated.h"

USTRUCT()
struct FClosestObject
{
	GENERATED_BODY();

public:
	class ALobbyInteractableBase* InteractableObject = nullptr;

	float Distance = 0;
};

UCLASS()
class LOCKED_API ALobbyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ALobbyCharacter();

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	//	class UCameraComponent* PerspectiveCamera;

	bool IsReady = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<class UGameSettingWidget> GameSettingWidgetClass;

	UPROPERTY(Replicated)
		FClosestObject NearestInteractable;
	UPROPERTY(Replicated)
		ALobbyInteractableBase* OldClosest;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveForward(float Value);
	void MoveRight(float Value);

	UFUNCTION(Client, Reliable)
		void SearchForNearestInterable();

	UFUNCTION(Client, Reliable)
		void OpenGameSettingMenu();

	UFUNCTION()
	void InteractWithNearestObject();

	UFUNCTION(Server, Reliable)
		void ReadyUp();
	UFUNCTION(Server, Reliable)
		void UnReady();
};
