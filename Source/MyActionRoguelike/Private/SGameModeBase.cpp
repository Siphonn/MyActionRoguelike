// Fill out your copyright notice in the Description page of Project Settings.

#include "SGameModeBase.h"
#include "AI/SAICharacter.h"
#include "EngineUtils.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "SActionComponent.h"
#include "SAttributeComponent.h"
#include "SCharacter.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "SGameplayInterface.h"
#include "SMonsterData.h"
#include "SPlayerState.h"
#include "SSaveGame.h"
#include "../MyActionRoguelike.h"
#include "Engine/AssetManager.h"


static TAutoConsoleVariable<bool> CVarSpawnBots(TEXT("su.SpawnBots"), true, TEXT("Enable spawning of bots using timer."), ECVF_Cheat);
static TAutoConsoleVariable<bool> CVarSpawnBotLocations(TEXT("su.SpawnBotsLocations"), false, TEXT("Show bots original spawn location"), ECVF_Cheat);


ASGameModeBase::ASGameModeBase()
{
	SpawnTimerInterval = 2.0f;
	CreditOnKill = 20;
	MaxNumberOfPowerUps = 10;
	PowerUpSpawnDistance = 1000.f;

	PlayerStateClass = ASPlayerState::StaticClass();

	SlotName = "SaveGame01";
}

void ASGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	LoadSaveGame();
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

void ASGameModeBase::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	// Called before Super:: so we set variables before 'BeginPlayingState' is called in PlayerController (where we instantiate Main_UI)
	ASPlayerState* PS = NewPlayer->GetPlayerState<ASPlayerState>();
	if (PS)
	{
		PS->LoadPlayerState(CurrentSaveGame);
	}

	Super::HandleStartingNewPlayer_Implementation(NewPlayer);
}

void ASGameModeBase::OnActorKilled(AActor* VictimActor, AActor* Killer)
{
	UE_LOG(LogTemp, Log, TEXT("OnActorKilled: Victim %s, Killer: %s"), *GetNameSafe(VictimActor), *GetNameSafe(Killer));

	ASCharacter* Player = Cast<ASCharacter>(VictimActor);
	if (Player)
	{
		FTimerHandle TimerHandle_RespawnDelay;
		FTimerDelegate Delegate;

		Delegate.BindUFunction(this, "RespawnPlayerElapsed", Player->GetController());

		float RespawnDelay = 2.0f;
		GetWorldTimerManager().SetTimer(TimerHandle_RespawnDelay, Delegate, RespawnDelay, false);
	}

	APawn* KillerPawn = Cast<APawn>(Killer);
	if (KillerPawn)
	{
		ASPlayerState* PlayerState = KillerPawn->GetPlayerState<ASPlayerState>();
		if (PlayerState)
		{
			PlayerState->AddCredit(CreditOnKill);
		}
	}
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
	if (Locations.IsValidIndex(0))
	{
		// Select spawned enemy from data table
		if (MonsterTable)
		{
			TArray<FMonsterInfoRow*> Rows;
			MonsterTable->GetAllRows("", Rows);

			// Get Random Enemy
			int32 RandomIndex = FMath::RandRange(0, Rows.Num() - 1);
			FMonsterInfoRow* SelectedRow = Rows[RandomIndex];


			UAssetManager* Manager = UAssetManager::GetIfValid();
			if (Manager)
			{
				LogToScreen(this, "Loading monster...", FColor::Green);
				
				TArray<FName> Bundles;
				FStreamableDelegate Delegate = FStreamableDelegate::CreateUObject(this, &ASGameModeBase::OnMonsterLoaded, SelectedRow->MonsterId, Locations[0]);
				Manager->LoadPrimaryAsset(SelectedRow->MonsterId, Bundles, Delegate);
			}
			//GetWorld()->SpawnActor<AActor>(MinionClass, Locations[0], FRotator::ZeroRotator);
		}

		// Console variable to track all the used spawn locations
		if (CVarSpawnBotLocations.GetValueOnGameThread())
		{
			UE_LOG(LogTemp, Warning, TEXT("Bot spawning location shown via cvar 'CVarSapwnBotsLocations'."));
			DrawDebugSphere(GetWorld(), Locations[0], 50.0f, 20, FColor::Blue, false, 60.0f);
		}
	}
}

void ASGameModeBase::OnMonsterLoaded(FPrimaryAssetId LoadedId, FVector SpawnLocation)
{
	LogToScreen(this, "Finished loading.", FColor::Green);
	
	UAssetManager* Manager = UAssetManager::GetIfValid();
	if (Manager)
	{
		USMonsterData* MonsterData = Cast<USMonsterData>(Manager->GetPrimaryAssetObject(LoadedId));
		if (MonsterData)
		{
			AActor* NewBot = GetWorld()->SpawnActor<AActor>(MonsterData->MonsterClass, SpawnLocation, FRotator::ZeroRotator);
			if (NewBot)
			{
				LogToScreen(this, FString::Printf(TEXT("Spawned enemy: %s (%s)"), *GetNameSafe(NewBot), *GetNameSafe(MonsterData)));

				// Grant special actions, buff, etc.
				USActionComponent* ActionComp = Cast<USActionComponent>(NewBot->GetComponentByClass(USActionComponent::StaticClass()));
				if (ActionComp)
				{
					for (TSubclassOf<USAction> ActionClass : MonsterData->Actions)
					{
						ActionComp->AddAction(NewBot, ActionClass);
					}
				}
			}
		}
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
			// TODO: current only work with 2 types of power-up. Random number between 0 & Powerups length (-1). Used Power at this index 
			// Select a random pickup to spawn.
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

void ASGameModeBase::WriteSaveGame()
{
	// Iterate all player states, we don't have proper ID to match yet (requires Steam or EQS)
	for (int32 i = 0; i < GameState->PlayerArray.Num(); i++)
	{
		ASPlayerState* PS = Cast<ASPlayerState>(GameState->PlayerArray[i]);
		if (PS)
		{
			PS->SavePlayerState(CurrentSaveGame);
			break; // single player only at this point
		}
	}

	CurrentSaveGame->SavedActors.Empty();

	// Iterate the entire world of Actors
	for (FActorIterator It(GetWorld()); It; ++It)
	{
		AActor* Actor = *It;
		// Only interest in our 'gameplay actors'
		if (!Actor->Implements<USGameplayInterface>())
		{
			continue;
		}

		FActorSaveData ActorData;
		ActorData.ActorName = Actor->GetName();
		ActorData.Transform = Actor->GetTransform();

		// Pass the array to fill with data from the Actor
		FMemoryWriter MemWriter(ActorData.ByteData);

		FObjectAndNameAsStringProxyArchive Ar(MemWriter, true);
		// Find only variables with UPROPERTY(SaveGame)
		Ar.ArIsSaveGame = true;
		// Converts Actor's SaveGame UPROPERTIES into binary array
		Actor->Serialize(Ar);

		CurrentSaveGame->SavedActors.Add(ActorData);
	}

	UGameplayStatics::SaveGameToSlot(CurrentSaveGame, SlotName, 0);
}

void ASGameModeBase::LoadSaveGame()
{
	if (UGameplayStatics::DoesSaveGameExist(SlotName, 0))
	{
		CurrentSaveGame = Cast<USSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));
		if (CurrentSaveGame == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to load SameGame Data."));
			return;
		}
		UE_LOG(LogTemp, Log, TEXT("Loaded SameGame Data."));


		// Iterate the entire world of Actors
		for (FActorIterator It(GetWorld()); It; ++It)
		{
			AActor* Actor = *It;
			// Only interest in our 'gameplay actors'
			if (!Actor->Implements<USGameplayInterface>())
			{
				continue;
			}

			for (FActorSaveData ActorData : CurrentSaveGame->SavedActors)
			{
				if (ActorData.ActorName == Actor->GetName())
				{
					Actor->SetActorTransform(ActorData.Transform);

					FMemoryReader MemReader(ActorData.ByteData);

					FObjectAndNameAsStringProxyArchive Ar(MemReader, true);
					Ar.ArIsSaveGame = true;
					// Converts binary array back into actor's variables
					Actor->Serialize(Ar);

					ISGameplayInterface::Execute_OnActorLoaded(Actor);

					break;
				}
			}
		}
	}
	else
	{
		CurrentSaveGame = Cast<USSaveGame>(UGameplayStatics::CreateSaveGameObject(USSaveGame::StaticClass()));
		UE_LOG(LogTemp, Log, TEXT("Created new SameGame Data."));
	}
}
