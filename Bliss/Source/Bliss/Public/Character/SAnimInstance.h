// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SAnimInstance.generated.h"

class ASCharacter;

UCLASS()
class BLISS_API USAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:
	/* Variables */
	// Derive from character input values
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		float InputX_Anim;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		float InputY_Anim;

	UPROPERTY(BlueprintReadWrite)
		float MovementDirection;

	bool bIsSprinting;

	// Ref to owner character 
	UPROPERTY(BlueprintReadOnly)
		ASCharacter* OwnerCharacter;

	// Local movement variables
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		bool bIsCrouching;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		bool bIsFalling;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		float MovementSpeed;

	UPROPERTY(BlueprintReadWrite)
		bool bReceivedInitialDirection;

public:
	/* Functions */
	// Called to update animation properties
	UFUNCTION(BlueprintCallable)
		void UpdateAnimationProperties(float DeltaTime);

	// Called in anim blueprint to assign direction value and receive initial direcion
	UFUNCTION(BlueprintImplementableEvent)
		void SetDirectionAndReceiveMovementDirection();

};
