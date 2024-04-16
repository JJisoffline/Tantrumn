// Fill out your copyright notice in the Description page of Project Settings.


#include "TantrumnPlayerController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TantrumnCharacterBase.h"
#include "TantrumnGameModeBase.h"

static TAutoConsoleVariable<bool> CVarDisplayLaunchInputDelta(
	TEXT("Tantrum.Character.Debug.DisplayLaunchInputDelta"),
	false,
	TEXT("Display Launch Input Delta"),
	ECVF_Default);

void ATantrumnPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (InputComponent)
	{
		InputComponent->BindAxis(TEXT("MoveForward"),
			                     this,
			                     &ATantrumnPlayerController::RequestMoveForward);
		InputComponent->BindAxis(TEXT("MoveRight"),
			                     this,
			                     &ATantrumnPlayerController::RequestMoveRight);
		InputComponent->BindAxis(TEXT("LookUp"),
								 this,
								 &ATantrumnPlayerController::RequestLookUp);
		InputComponent->BindAxis(TEXT("LookRight"),
								 this,
								 &ATantrumnPlayerController::RequestLookRight);
		InputComponent->BindAction(TEXT("Jump"),
								   EInputEvent::IE_Pressed,
								   this,
								   &ATantrumnPlayerController::RequestJumpStart);
		InputComponent->BindAction(TEXT("Jump"),
			                       EInputEvent::IE_Released,
			                       this,
			                       &ATantrumnPlayerController::RequestJumpStop);
		InputComponent->BindAction(TEXT("Crouch"),
								   EInputEvent::IE_Pressed,
								   this,
								   &ATantrumnPlayerController::RequestCrouchStart);
		InputComponent->BindAction(TEXT("Crouch"),
							   	   EInputEvent::IE_Released,
								   this,
								   &ATantrumnPlayerController::RequestCrouchStop);
		InputComponent->BindAction(TEXT("Sprint"),
								   EInputEvent::IE_Pressed,
								   this,
								   &ATantrumnPlayerController::RequestSprintStart);
		InputComponent->BindAction(TEXT("Sprint"),
								   EInputEvent::IE_Released,
								   this,
								   &ATantrumnPlayerController::RequestSprintStop);
		InputComponent->BindAction(TEXT("PullObject"),
								   EInputEvent::IE_Pressed,
								   this,
								   &ATantrumnPlayerController::RequestPullObjectStart);
		InputComponent->BindAction(TEXT("PullObject"),
								   EInputEvent::IE_Released,
								   this,
								   &ATantrumnPlayerController::RequestPullObjectStop);
		InputComponent->BindAction(TEXT("ThrowObjectMouse"),
								   EInputEvent::IE_Pressed,
								   this,
								   &ATantrumnPlayerController::RequestThrowObject);
		InputComponent->BindAxis(TEXT("ThrowObjectGP"),
								 this,
								 &ATantrumnPlayerController::RequestThrowObject);
	}
}

void ATantrumnPlayerController::BeginPlay()
{
	Super::BeginPlay();

	GameModeRef = Cast<ATantrumnGameModeBase>(GetWorld()->GetAuthGameMode());

	//if (HUDClass)
	//{
		//HUDWidget = CreateWidget(this, HUDClass);
		//if (HUDWidget)
		//{
			//HUDWidget->AddToViewport();
		//}
	//}

	if (GetCharacter())
	{
		DefaultWalkSpeed = GetCharacter()->GetCharacterMovement()->MaxWalkSpeed;
	}
}

void ATantrumnPlayerController::RequestMoveForward(float AxisValue)
{
	FRotator const ControlSpaceRot = GetControlRotation();
	GetPawn()->AddMovementInput(FRotationMatrix(ControlSpaceRot).GetScaledAxis(EAxis::X), AxisValue);
}

void ATantrumnPlayerController::RequestMoveRight(float AxisValue)
{
	FRotator const ControlSpaceRot = GetControlRotation();
	GetPawn()->AddMovementInput(FRotationMatrix(ControlSpaceRot).GetScaledAxis(EAxis::Y), AxisValue);
}

void ATantrumnPlayerController::RequestLookUp(float AxisValue)
{
	AddPitchInput(AxisValue * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ATantrumnPlayerController::RequestLookRight(float AxisValue)
{
	AddYawInput(AxisValue * BaseLookRightRate * GetWorld()->GetDeltaSeconds());
}

void ATantrumnPlayerController::RequestJumpStart()
{
	if (GetCharacter())
	{
		GetCharacter()->Jump();
	}
}

void ATantrumnPlayerController::RequestJumpStop()
{
	if (GetCharacter())
	{
		GetCharacter()->StopJumping();
	}
}

void ATantrumnPlayerController::RequestCrouchStart()
{
	if (!GetCharacter()->GetCharacterMovement()->IsMovingOnGround()) { return; }
	if (GetCharacter())
	{
		GetCharacter()->Crouch();
	}
}

void ATantrumnPlayerController::RequestCrouchStop()
{
	if (GetCharacter())
	{
		GetCharacter()->UnCrouch();
	}
}

void ATantrumnPlayerController::RequestSprintStart()
{
	if (GetCharacter())
	{
		GetCharacter()->GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	}
}

void ATantrumnPlayerController::RequestSprintStop()
{
	if (GetCharacter())
	{
		GetCharacter()->GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;
	}
}

void ATantrumnPlayerController::RequestThrowObject()
{
	if (ATantrumnCharacterBase* TantrumnCharacterBase = Cast<ATantrumnCharacterBase>(GetCharacter()))
	{
		TantrumnCharacterBase->RequestThrowObject();
	}
}

void ATantrumnPlayerController::RequestThrowObject(float AxisValue)
{
	if (ATantrumnCharacterBase* TantrumnCharacterBase = Cast<ATantrumnCharacterBase>(GetCharacter()))
	{
		if (TantrumnCharacterBase->CanThrowObject())
		{
			float currentDelta = AxisValue = LastAxis;

			//debug
			if (CVarDisplayLaunchInputDelta->GetBool())
			{
				if (fabs(currentDelta) > 0.0f)
				{
					UE_LOG(LogTemp, Warning, TEXT("Axis: %f currentDelta %f"), AxisValue, LastAxis);
				}
			}
			LastAxis = AxisValue;
			const bool IsFlick = fabs(currentDelta) > FlickThreshold;
			if (IsFlick)
			{
				TantrumnCharacterBase->RequestThrowObject();
			}
		}
		else
		{
			LastAxis = 0.0f;
		}
	}
}

void ATantrumnPlayerController::RequestPullObjectStart()
{
	if (ATantrumnCharacterBase* TantrumnCharacterBase = Cast<ATantrumnCharacterBase>(GetCharacter()))
	{
		TantrumnCharacterBase->RequestPullObjectStart();
	}
}

void ATantrumnPlayerController::RequestPullObjectStop()
{
	if (ATantrumnCharacterBase* TantrumnCharacterBase = Cast<ATantrumnCharacterBase>(GetCharacter()))
	{
		TantrumnCharacterBase->RequestPullObjectStop();
	}
}
