// Fill out your copyright notice in the Description page of Project Settings.


#include "TantrumnGameModeBase.h"
#include "Kismet/GameplayStatics.h"

ATantrumnGameModeBase::ATantrumnGameModeBase()
{
}

void ATantrumnGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	CurrentGameState = EGameState::Waiting;
	DisplayCountdown();
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ATantrumnGameModeBase::StartGame, GameCountdownDuration, false);
}

EGameState ATantrumnGameModeBase::GetCurrentGameState() const
{
	return CurrentGameState;
}

void ATantrumnGameModeBase::PlayerReachedEnd()
{
	CurrentGameState = EGameState::GameOver;

	GameWidget->LevelComplete();
	FInputModeUIOnly InputMode;
	PC->SetInputMode(InputMode);
	PC->SetShowMouseCursor(true);
}

void ATantrumnGameModeBase::DisplayCountdown()
{
	if (!GameWidgetClass) { return; }
	
	PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	GameWidget = CreateWidget<UTantrumnGameWidget>(PC, GameWidgetClass);
	GameWidget->AddToViewport();
	GameWidget->StartCountdown(GameCountdownDuration, this);
}

void ATantrumnGameModeBase::StartGame()
{
	CurrentGameState = EGameState::Playing;

//	FInputModeGameOnly InputMode;
//	PC->SetInputMode(InputMode); <-- Something happening here.
//	PC->SetShowMouseCursor(false);
// 
//  JJTD This part was written to allow buttons to become pressable if the player decides to click replay, currently broken because PC gets put to nullptr.
}
