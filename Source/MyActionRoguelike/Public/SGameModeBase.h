// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "GameFramework/GameModeBase.h"
#include "SGameModeBase.generated.h"

class UEnvQuery;
class UEnvQueryInstanceBlueprintWrapper;
class UCurveFloat;

/**
 * 
 */
UCLASS()
class MYACTIONROGUELIKE_API ASGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category="AI")
	TSubclassOf<AActor> MinionClass;
	UPROPERTY(EditDefaultsOnly, Category="AI")
	UEnvQuery* SpawnBotQuery;
	UPROPERTY(EditDefaultsOnly, Category="AI")
	UCurveFloat* DifficultyCurve;
	FTimerHandle TimerHandle_SpawnBots;
	UPROPERTY(EditDefaultsOnly, Category="AI")
	float SpawnTimerInterval;
	UPROPERTY(EditDefaultsOnly, Category="AI")
	int32 CreditOnKill;

	UPROPERTY(EditDefaultsOnly,Category="Power Ups")
	TArray<TSubclassOf<AActor>> PowerUps;
	UPROPERTY(EditDefaultsOnly, Category="Power Ups")
	UEnvQuery* PowerUpQuery;
	UPROPERTY(EditDefaultsOnly, Category="Power Ups")
	int32 MaxNumberOfPowerUps;
	UPROPERTY(EditDefaultsOnly, Category="Power Ups")
	float PowerUpSpawnDistance;

	UFUNCTION()
	void SpawnBotTimerElapsed();
	UFUNCTION()
	void OnSpawnBotQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);
	UFUNCTION()
	void OnSpawnPowerUpQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);
	UFUNCTION()
	void RespawnPlayerElapsed(AController* Controller);

public:
	ASGameModeBase();

	virtual void StartPlay() override;

	virtual void OnActorKilled(AActor* VictimActor, AActor* Killer);
	
	UFUNCTION(Exec)
	void KillAll();
};
