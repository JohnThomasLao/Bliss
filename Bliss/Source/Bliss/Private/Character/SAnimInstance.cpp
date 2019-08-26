// Fill out your copyright notice in the Description page of Project Settings.

#include "SAnimInstance.h"
#include "Engine/GameEngine.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "SCharacter.h"
#include "TimerManager.h"

void USAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	// Get owner pawn
	APawn* OwnerPawn = TryGetPawnOwner();

	if (OwnerPawn != NULL) {
		// Update animation properties using pawn movement component
		bIsCrouching = OwnerPawn->GetMovementComponent()->IsCrouching();
		bIsFalling = OwnerPawn->GetMovementComponent()->IsFalling();

		// Cast owner pawn to owner character
		OwnerCharacter = Cast<ASCharacter>(OwnerPawn);

		if (OwnerCharacter != NULL) {
			// Update animation properties using character input values
			InputX_Anim = OwnerCharacter->InputX;
			InputY_Anim = OwnerCharacter->InputY;
			bIsSprinting = OwnerCharacter->bIsSprinting;

			if (bIsSprinting) {
				MovementSpeed = FMath::Clamp(UKismetMathLibrary::Abs(InputX_Anim) + UKismetMathLibrary::Abs(InputY_Anim), 0.f, 1.f) * 2.f; // Speed up twice
			}
			else {
				MovementSpeed = FMath::Clamp(UKismetMathLibrary::Abs(InputX_Anim) + UKismetMathLibrary::Abs(InputY_Anim), 0.f, 1.f); // Speed stays the same
			}

			// Debug movement speed
			if (GEngine != NULL) {
				GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Red, FString::Printf(TEXT("Movement Speed: %f"), MovementSpeed));
			}

			if (MovementSpeed > 0.01f) { // If speed > 0.01 then set direction input to anim
				if (bReceivedInitialDirection) {
					if (MovementSpeed < 0.01f) { // If speed < 0.01 then reset received initial direction for next event
						bReceivedInitialDirection = false;
					}
				}
				else {
					MovementDirection = OwnerCharacter->MovementDirection; // Assign movement direction

					// Receive initial direction after a delay of 0.1 second
					GetWorld()->GetTimerManager().SetTimer(TimerHandle_ReceiveInitialDirection, this, &USAnimInstance::ReceiveInitialDirection, 0.1f, false);
				}
			}
			else {
				if (MovementSpeed < 0.01f) { // If speed < 0.01 then reset received initial direction for next event
					bReceivedInitialDirection = false;
				}
			}

		}
	}

}

void USAnimInstance::ReceiveInitialDirection()
{
	bReceivedInitialDirection = true;

}
