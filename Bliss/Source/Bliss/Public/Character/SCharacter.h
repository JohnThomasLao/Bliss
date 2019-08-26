// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

class UBoxComponent;
class UCameraComponent;
class USpringArmComponent;

UCLASS()
class BLISS_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/* Components */
	// Spring arm component that control the camera component
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
		USpringArmComponent* SpringArmComp;
	
	// Camera component
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
		UCameraComponent* CameraComp;

	// Pickups overlapping component (POC)
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
		UBoxComponent* PickupsOverlappingComp;

	/* Functions */
	// Movement inputs
	void MoveX(float Value);
	void MoveY(float Value);

	void JumpStart();
	void JumpEnd();

	void ToggleCrouch();

	void SprintStart();
	void SprintEnd();

	// Camera inputs
	void CameraX(float Value);
	void CameraY(float Value);

	// Events
	void CalculateMovementDirection(float InputX, float InputY);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/* Variables */
	// Movement input values
	float InputX;
	float InputY;

	UPROPERTY(BlueprintReadWrite)
		float MovementDirection;

	bool bIsSprinting;

};
