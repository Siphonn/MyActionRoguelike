// Fill out your copyright notice in the Description page of Project Settings.

#include "SGameModeBase.h"
#include "AI/SAICharacter.h"
#include "EngineUtils.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "SAttributeComponent.h"
#include "SCharacter.h"
#include "SPlayerState.h"


static TAutoConsoleVariable<bool> CVarSpawnBots(TEXT("su.SpawnBots"), true, TEXT("Enable spawning of bots using timer."), ECVF_Cheat);


ASGameModeBase::ASGameModeBase()
{
	SpawnTimerInterval = 2.0f;
	CreditOnKill = 20;
	MaxNumberOfPowerUps = 10;
	PowerUpSpawnDistance = 1000.f;
}

void ASGameModeBase::StartPlay()
{
	Super::StartPlay();

	// Continuously timer to spawns more bots
	// Actual amount of bots and whether its allowed to spawn determined by spawn logic later in chain.....
	GetWorldTimerManager().SetTimer(TimerHandle_SpawnBots, this, &ASGameModeBase::SpawnBotTimerElapsed, SpawnTimerInterval, true);

	// Check for PowerUps
	if (ensure(PowerUps.Num()>0))
	{
		// Rune EQS query to find location for bot to spawn. Spawn actor done in OnSpawnBotQueryCompleted events
		UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(this, PowerUpQuery, this, EEnvQueryRunMode::AllMatching, nullptr);
		if (ensure(QueryInstance))
		{
			QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &ASGameModeBase::OnSpawnPowerUpQueryCompleted);
		}
	}
}

void ASGameModeBase::OnActorKilled(AActor* VictimActor, AActor* Killer)
{
	ASCharacter* Player = Cast<ASCharacter>(VictimActor);
	if (Player)
	{
		FTimerHandle TimerHandle_RespawnDelay;
		FTimerDelegate Delegate;

		Delegate.BindUFunction(this, "RespawnPlayerElapsed", Player->GetController());

		float RespawnDelay = 2.0f;
		GetWorldTimerManager().SetTimer(TimerHandle_RespawnDelay, Delegate, RespawnDelay, false);
	}
	else
	{
		ASCharacter* PlayerCharacter = Cast<ASCharacter>(Killer);
		if (PlayerCharacter)
		{
			ASPlayerState* PlayerState = PlayerCharacter->GetPlayerState<ASPlayerState>();
			if (PlayerState)
			{
				PlayerState->AddCredit(CreditOnKill);
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("OnActorKilled: Victim %s, Killer: %s"), *GetNameSafe(VictimActor), *GetNameSafe(Killer));
}

void ASGameModeBase::KillAll()
{
	for (TActorIterator<ASAICharacter> It(GetWorld()); It; ++It)
	{
		ASAICharacter* Bot = *It;

		USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(Bot);
		if (ensure(AttributeComp) && AttributeComp->IsAlive())
		{
			AttributeComp->Kill(this); // TODO: pass in player? for kill credit
		}
	}
}

void ASGameModeBase::SpawnBotTimerElapsed()
{
	// CVAR
	if (!CVarSpawnBots.GetValueOnGameThread())
	{
		UE_LOG(LogTemp, Warning, TEXT("Bot spawning disabled via cvar 'CVarSapwnBots'."));
		return;
	}

	// Find all bots (currently alive)
	int32 NumberOfALiveBots = 0;
	for (TActorIterator<ASAICharacter> It(GetWorld()); It; ++It)
	{
		ASAICharacter* Bot = *It;

		USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(Bot);
		if (ensure(AttributeComp) && AttributeComp->IsAlive())
		{
			NumberOfALiveBots++;
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Found %i alive bots."), NumberOfALiveBots);

	// Max bots
	float MaxBotCount = 10.0f;
	if (DifficultyCurve)
	{
		MaxBotCount = DifficultyCurve->GetFloatValue(GetWorld()->TimeSeconds);
	}

	// Spot spawn if at Max bots
	if (NumberOfALiveBots >= MaxBotCount)
	{
		UE_LOG(LogTemp, Log, TEXT("At maximum bot spawn capacity. Skipping next spawn"));
		return;
	}

	// Rune EQS query to find location for bot to spawn. Spawn actor done in OnSpawnBotQueryCompleted events
	UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(this, SpawnBotQuery, this, EEnvQueryRunMode::RandomBest5Pct, nullptr);
	if (ensure(QueryInstance))
	{
		QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &ASGameModeBase::OnSpawnBotQueryCompleted);
	}
}

void ASGameModeBase::OnSpawnBotQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	if (QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawn bot EQS Query failed!!"))
		return;
	}

	TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();
	if (Locations.Num() > 0)
	{
		GetWorld()->SpawnActor<AActor>(MinionClass, Locations[0], FRotator::ZeroRotator);

		// Track all the used spawn locations
		DrawDebugSphere(GetWorld(), Locations[0], 50.0f, 20, FColor::Blue, false, 60.0f);
	}
}

void ASGameModeBase::OnSpawnPowerUpQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	if (QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawn bot EQS Query failed!!"))
		return;
	}

	TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();
	//UE_LOG(LogTemp, Warning, TEXT("Number of possible PowerUp spawn locations is : %d"), Locations.Num());

	TArray<FVector> UsedLocations;
	int32 SpawnCounter = 0;

	while (SpawnCounter < MaxNumberOfPowerUps && Locations.Num() > 0)
	{
		int32 RandomNumber = FMath::RandRange(0, Locations.Num() - 1);
		FVector PickedLocation = Locations[RandomNumber];
		Locations.RemoveAt(RandomNumber);

		// Make sure the picked location is far enough distance away from an already used location.
		bool bValidLocation = true;
		for (FVector OtherLocation : UsedLocations)
		{
			float Distance = (PickedLocation - OtherLocation).Size();

			if (Distance < PowerUpSpawnDistance)
			{
				// Too close to a Used Location
				bValidLocation = false;
				break;
			}
		}

		if (bValidLocation)
		{
			SpawnCounter++;
			// Select a random pickup to spawn
			TSubclassOf<AActor> RandomPowerUp = FMath::RandBool() ? PowerUps[0] : PowerUps[1];
			GetWorld()->SpawnActor<AActor>(RandomPowerUp, PickedLocation, FRotator::ZeroRotator);
		}
	}
}

void ASGameModeBase::RespawnPlayerElapsed(AController* Controller)
{
	if (ensure(Controller))
	{
		Controller->UnPossess();

		RestartPlayer(Controller);
	}
}
