// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TantrumnGameWidget.h"
#include "TantrumnGameModeBase.generated.h"

// Enum to track the curret state of the game
UENUM(BlueprintType)
enum class EGameState : uint8
{
	NONE      UMETA(DisplayName = "NONE"),
	Waiting   UMETA(DisplayName = "Waiting"),
	Playing   UMETA(DisplayName = "Playing"),
	Paused    UMETA(DisplayName = "Paused"),
	GameOver  UMETA(DisplayName = "GameOver"),
};


UCLASS()
class TANTRUMN_API ATantrumnGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ATantrumnGameModeBase();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	EGameState GetCurrentGameState() const;
	void PlayerReachedEnd();

private:
	UPROPERTY(VisibleAnywhere, Category = "States")
	EGameState CurrentGameState = EGameState::NONE;

	// Countdown before Game Start
	UPROPERTY(EditAnywhere, Category = "Game Details")
	float GameCountdownDuration = 4.0f;

	UPROPERTY()
	UTantrumnGameWidget* GameWidget;
	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<UTantrumnGameWidget> GameWidgetClass;

	APlayerController* PC = nullptr;

	FTimerHandle TimerHandle;

	void DisplayCountdown();
	void StartGame();
};
