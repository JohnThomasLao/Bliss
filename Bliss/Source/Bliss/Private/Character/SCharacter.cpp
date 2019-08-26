// Fill out your copyright notice in the Description page of Project Settings.

#include "SCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Engine/GameEngine.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Automatically possess this character
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	// Disable all the character controller rotations
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement settings
	GetCharacterMovement()->bOrientRotationToMovement = true; // Movement character toward camera pointing direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 250.f, 0.f);
	GetCharacterMovement()->JumpZVelocity = 250.f;
	GetCharacterMovement()->AirControl = 0.2f;
	GetCharacterMovement()->MaxWalkSpeed = 820.f;
	GetCharacterMovement()->MaxWalkSpeedCrouched = 740.f;
	GetCharacterMovement()->MaxAcceleration = 800.f;
	GetCharacterMovement()->BrakingFriction = 0.2f;

	// Initialize components and variables
	/* Components */
	// Spring arm component
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->TargetArmLength = 370.f;
	SpringArmComp->bEnableCameraLag = true;
	SpringArmComp->CameraLagSpeed = 5.f;
	SpringArmComp->bEnableCameraRotationLag = true;
	SpringArmComp->CameraRotationLagSpeed = 8.f;

	// Camera component
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName); // Attach to the 'tail' of the spring arm component
	CameraComp->bUsePawnControlRotation = false;
	
	// Pickups overlapping component (POC)
	PickupsOverlappingComp = CreateDefaultSubobject<UBoxComponent>(TEXT("PickupsOverlappingComponent"));
	PickupsOverlappingComp->SetupAttachment(RootComponent);

	/* Variables */
	bIsSprinting = false;

}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// Set POC size to match capsule height and extends to a bit far away from root component
	PickupsOverlappingComp->SetBoxExtent(FVector(200.f, 200.f, GetCapsuleComponent()->GetScaledCapsuleHalfHeight()), true);

}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Constantly calculate character movement direction
	CalculateMovementDirection(InputX, InputY);

	// Debug movement direction
	if (GEngine != NULL) {
		GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Emerald, FString::Printf(TEXT("Movement Direction: %f"), MovementDirection));
	}

}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Setup character input bindings
	// Movement inputs
	PlayerInputComponent->BindAxis("MoveX", this, &ASCharacter::MoveX);

	PlayerInputComponent->BindAxis("MoveY", this, &ASCharacter::MoveY);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASCharacter::JumpStart);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ASCharacter::JumpEnd);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ASCharacter::SprintStart);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ASCharacter::SprintEnd);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ASCharacter::ToggleCrouch);

	// Camera inputs
	PlayerInputComponent->BindAxis("CameraX", this, &ASCharacter::CameraX);

	PlayerInputComponent->BindAxis("CameraY", this, &ASCharacter::CameraY);

}

void ASCharacter::MoveX(float Value)
{
	InputX = Value;

	// Debug input x value
	if (GEngine != NULL) {
		GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Blue, FString::Printf(TEXT("Input X: %f"), InputX));
	}

	if (Value != 0.f && Controller != NULL) {
		if (bIsSprinting) {
			Value *= 2.f;
		}

		// Determine the yaw-value of the controller rotation
		FRotator NewRotation = Controller->GetControlRotation();
		FRotator NewYawRotation = FRotator(0.f, NewRotation.Yaw, 0.f);

		// Make direction base on forward unit axis of the yaw rotation
		FVector Direction = FRotationMatrix(NewYawRotation).GetUnitAxis(EAxis::X);

		// Move character using created direction
		AddMovementInput(Direction, Value / 3, false);
	}

}

void ASCharacter::MoveY(float Value)
{
	InputY = Value;

	// Debug input y value
	if (GEngine != NULL) {
		GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Blue, FString::Printf(TEXT("Input Y: %f"), InputY));
	}

	if (Value != 0.f && Controller != NULL) {
		if (bIsSprinting) {
			Value *= 2.f;
		}

		// Determine the yaw-value of the controller rotation
		FRotator NewRotation = Controller->GetControlRotation();
		FRotator NewYawRotation = FRotator(0.f, NewRotation.Yaw, 0.f);

		// Make direction base on right unit axis of the yaw rotation
		FVector Direction = FRotationMatrix(NewYawRotation).GetUnitAxis(EAxis::Y);

		// Move character using created direction
		AddMovementInput(Direction, Value / 3, false);
	}

}

void ASCharacter::JumpStart()
{
	Jump();

}

void ASCharacter::JumpEnd()
{
	StopJumping();

}

void ASCharacter::ToggleCrouch()
{
	if (GetCharacterMovement()->IsCrouching()) { // If character is found already in crouching state
		UnCrouch();
	}
	else {
		Crouch();
	}

}

void ASCharacter::SprintStart()
{
	bIsSprinting = true;

}

void ASCharacter::SprintEnd()
{
	bIsSprinting = false;

}

void ASCharacter::CameraX(float Value)
{
	AddControllerYawInput(Value); // Control camera horizontally

}

void ASCharacter::CameraY(float Value)
{
	AddControllerPitchInput(Value); // Control camera vertically

}

void ASCharacter::CalculateMovementDirection(float InputX, float InputY)
{
	// Make vector using movement input values - called input vector
	FVector InputVector = UKismetMathLibrary::MakeVector(InputX, InputY * -1.f, 0.f);

	// Make rotation using x-value of input vector - called input rotation
	FRotator InputRotation = UKismetMathLibrary::MakeRotFromX(InputVector);

	// Make rotation using rotations of the camera component and capsule component - called comp rotation
	FRotator CompRotation = UKismetMathLibrary::NormalizedDeltaRotator(CameraComp->GetComponentRotation(), GetCapsuleComponent()->GetComponentRotation());

	// Make rotation using input rotation and comp rotation - called direction rotation
	FRotator DirectionRotation = UKismetMathLibrary::NormalizedDeltaRotator(CompRotation, InputRotation);

	// Assign movement direction as the yaw-value of the direction rotation
	MovementDirection = DirectionRotation.Yaw;

}
