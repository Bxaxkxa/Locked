// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PlayerControlPawn.generated.h"


UCLASS()
class MYPROJECT_API APlayerControlPawn : public APawn
{
	GENERATED_BODY()

public:
	DECLARE_DELEGATE_OneParam(FTestingDelegate, FString);
	// Sets default values for this pawn's properties
	APlayerControlPawn();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class USceneComponent* RootScene;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
		float CameraFollowSpeed;

	UPROPERTY(EditAnywhere, Category = Template, meta = (AllowPrivateAccess = "true"))
		TSubclassOf<class ALockedCharacter> CharacterTemplate;

	ALockedCharacter* ControlledCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		class ARoomTile* CurrentRoom;

	void SpawnControlledCharacter();

	bool bStillInMove = false;
	bool bIsFollowBehaviour = true;

	FVector TargetCameraLocation;

	FTimerHandle MovementTimer;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveCharacterTo(FString MoveToDirection);

	void MoveCameraTo(ARoomTile* MoveToDirection);

	void ChangeCameraBehaviour();
};
