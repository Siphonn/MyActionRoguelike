// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "GameFramework/GameModeBase.h"
#include "SGameModeBase.generated.h"

class UCurveFloat;
class UDataTable;
class UEnvQuery;
class UEnvQueryInstanceBlueprintWrapper;
class USMonsterData;
class USSaveGame;


/* DataTable type for spawning monsters in game mode */
USTRUCT(BlueprintType)
struct FMonsterInfoRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	FMonsterInfoRow()
	{
		Weight = 1.0f;
		SpawnCost = 5.0f;
		KillReward = 20.0f;
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FPrimaryAssetId MonsterId;
	//USMonsterData* MonsterData;
	//TSubclassOf<AActor> MonsterClass;

	/* Relative chance to pick this monster */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Weight;

	/* Points required by gamemode to spawn this unit */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SpawnCost;

	/* Amount of credits awarded for killing this unit */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float KillReward;
};

/**
 * 
 */
UCLASS()
class MYACTIONROGUELIKE_API ASGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	ASGameModeBase();

	// InitGame is called before any other function.
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	virtual void StartPlay() override;

	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

	virtual void OnActorKilled(AActor* VictimActor, AActor* Killer);

	UFUNCTION(Exec)
	void KillAll();

	UFUNCTION(BlueprintCallable, Category="SaveGame")
	void WriteSaveGame();

	void LoadSaveGame();

protected:
	/* All available monsters */
	UPROPERTY(EditDefaultsOnly, Category="AI")
	UDataTable* MonsterTable;
	// UPROPERTY(EditDefaultsOnly, Category="AI")
	// TSubclassOf<AActor> MinionClass;
	UPROPERTY(EditDefaultsOnly, Category="AI")
	UEnvQuery* SpawnBotQuery;
	UPROPERTY(EditDefaultsOnly, Category="AI")
	UCurveFloat* DifficultyCurve;
	FTimerHandle TimerHandle_SpawnBots;
	UPROPERTY(EditDefaultsOnly, Category="AI")
	float SpawnTimerInterval;
	UPROPERTY(EditDefaultsOnly, Category="AI")
	int32 CreditOnKill;

	UPROPERTY(EditDefaultsOnly, Category="Power Ups")
	TArray<TSubclassOf<AActor>> PowerUps;
	UPROPERTY(EditDefaultsOnly, Category="Power Ups")
	UEnvQuery* PowerUpQuery;
	UPROPERTY(EditDefaultsOnly, Category="Power Ups")
	int32 MaxNumberOfPowerUps;
	UPROPERTY(EditDefaultsOnly, Category="Power Ups")
	float PowerUpSpawnDistance;

	// SaveGame
	FString SlotName;
	UPROPERTY()
	USSaveGame* CurrentSaveGame;

	UFUNCTION()
	void SpawnBotTimerElapsed();
	UFUNCTION()
	void OnSpawnBotQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);
	UFUNCTION()
	void OnMonsterLoaded(FPrimaryAssetId LoadedId, FVector SpawnLocation);
	UFUNCTION()
	void OnSpawnPowerUpQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);
	UFUNCTION()
	void RespawnPlayerElapsed(AController* Controller);
};
